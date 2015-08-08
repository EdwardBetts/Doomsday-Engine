/** @file modelrenderer.cpp  Model renderer.
 *
 * @authors Copyright (c) 2014 Jaakko Keränen <jaakko.keranen@iki.fi>
 *
 * @par License
 * GPL: http://www.gnu.org/licenses/gpl.html
 *
 * <small>This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details. You should have received a copy of the GNU
 * General Public License along with this program; if not, see:
 * http://www.gnu.org/licenses</small>
 */

#include "render/modelrenderer.h"
#include "gl/gl_main.h"
#include "render/rend_main.h"
#include "render/vissprite.h"
#include "world/p_players.h"
#include "world/clientmobjthinkerdata.h"
#include "clientapp.h"

#include <de/filesys/AssetObserver>
#include <de/App>
#include <de/ModelBank>
#include <de/ScriptedInfo>

using namespace de;

static String const DEF_ANIMATION   ("animation");
static String const DEF_MATERIAL    ("material");
static String const DEF_UP_VECTOR   ("up");
static String const DEF_FRONT_VECTOR("front");
static String const DEF_AUTOSCALE   ("autoscale");
static String const DEF_MIRROR      ("mirror");

DENG2_PIMPL(ModelRenderer)
, DENG2_OBSERVES(filesys::AssetObserver, Availability)
, DENG2_OBSERVES(Bank, Load)
, DENG2_OBSERVES(ModelDrawable, AboutToGLInit)
{
#define MAX_LIGHTS  4

    filesys::AssetObserver observer { "model\\..*" };
    ModelBank bank;
    std::unique_ptr<AtlasTexture> atlas;
    GLProgram program; /// @todo Specific models may want to use a custom program.
    GLUniform uMvpMatrix        { "uMvpMatrix",        GLUniform::Mat4 };
    GLUniform uTex              { "uTex",              GLUniform::Sampler2D };
    GLUniform uEyePos           { "uEyePos",           GLUniform::Vec3 };
    GLUniform uAmbientLight     { "uAmbientLight",     GLUniform::Vec4 };
    GLUniform uLightDirs        { "uLightDirs",        GLUniform::Vec3Array, MAX_LIGHTS };
    GLUniform uLightIntensities { "uLightIntensities", GLUniform::Vec4Array, MAX_LIGHTS };
    Matrix4f inverseLocal;
    int lightCount = 0;
    Id defaultNormals  { Id::None };
    Id defaultEmission { Id::None };
    Id defaultSpecular { Id::None };

    Instance(Public *i) : Base(i)
    {
        observer.audienceForAvailability() += this;
        bank.audienceForLoad() += this;
    }

    void init()
    {
        ClientApp::shaders().build(program, "model.skeletal.normal_specular_emission")
                << uMvpMatrix
                << uTex
                << uEyePos
                << uAmbientLight
                << uLightDirs
                << uLightIntensities;

        atlas.reset(AtlasTexture::newWithKdTreeAllocator(
                    Atlas::DefaultFlags,
                    GLTexture::maximumSize().min(GLTexture::Size(4096, 4096))));
        atlas->setBorderSize(1);
        atlas->setMarginSize(0);

        // Fallback normal map for models who don't provide one.
        QImage img(QSize(1, 1), QImage::Format_ARGB32);
        img.fill(qRgba(127, 127, 255, 255)); // z+
        defaultNormals = atlas->alloc(img);

        // Fallback emission map for models who don't have one.
        img.fill(qRgba(0, 0, 0, 0));
        defaultEmission = atlas->alloc(img);

        // Fallback specular map (no specular reflections).
        img.fill(qRgba(0, 0, 0, 0));
        defaultSpecular = atlas->alloc(img);

        uTex = *atlas;

        /*
        // All loaded items should use this atlas.
        bank.iterate([this] (DotPath const &path)
        {
            if(bank.isLoaded(path))
            {
                setupModel(bank.model(path));
            }
        });*/
    }

    void deinit()
    {
        // GL resources must be accessed from the main thread only.
        bank.unloadAll(Bank::ImmediatelyInCurrentThread);

        atlas.reset();
        program.clear();
    }

    void assetAvailabilityChanged(String const &identifier, filesys::AssetObserver::Event event)
    {
        //qDebug() << "loading model:" << identifier << event;

        if(event == filesys::AssetObserver::Added)
        {
            bank.add(identifier, App::asset(identifier).absolutePath("path"));

            // Begin loading the model right away.
            bank.load(identifier);
        }
        else
        {
            bank.remove(identifier);
        }
    }

    /**
     * Configures a ModelDrawable with the appropriate atlas and GL program.
     *
     * @param model  Model to configure.
     */
    void setupModel(ModelDrawable &model)
    {
        if(atlas)
        {
            model.setAtlas(*atlas);

            model.setDefaultTexture(ModelDrawable::Normals,  defaultNormals);
            model.setDefaultTexture(ModelDrawable::Emissive, defaultEmission);
            model.setDefaultTexture(ModelDrawable::Specular, defaultSpecular);

            // Use the texture mapping specified in the shader. This has to be done
            // only now because earlier we may not have the shader available yet.
            Record const &def = ClientApp::shaders()["model.skeletal.normal_specular_emission"];
            if(def.has("textureMapping"))
            {
                ModelDrawable::Mapping mapping;
                for(Value const *value : def.geta("textureMapping").elements())
                {
                    mapping << ModelDrawable::textToTextureMap(value->asText());
                }
                //qDebug() << "using mapping" << mapping;
                model.setTextureMapping(mapping);
            }
        }
        else
        {
            model.unsetAtlas();
        }
        model.setProgram(program);
    }

    void modelAboutToGLInit(ModelDrawable &model)
    {
        setupModel(model);
    }

    /**
     * When model assets have been loaded, we can parse their metadata to see if there
     * are any animation sequences defined. If so, we'll set up a shared lookup table
     * that determines which sequences to start in which mobj states.
     *
     * @param path  Model asset id.
     */
    void bankLoaded(DotPath const &path)
    {
        // Models use the shared atlas.
        ModelDrawable &model = bank.model(path);
        model.audienceForAboutToGLInit() += this;

        auto const asset = App::asset(path);

        std::unique_ptr<AuxiliaryData> aux(new AuxiliaryData);

        // Determine the coordinate system of the model.
        Vector3f front(0, 0, 1);
        Vector3f up   (0, 1, 0);
        if(asset.has(DEF_FRONT_VECTOR))
        {
            front = Vector3f(asset.geta(DEF_FRONT_VECTOR));
        }
        if(asset.has(DEF_UP_VECTOR))
        {
            up = Vector3f(asset.geta(DEF_UP_VECTOR));
        }
        bool mirror = ScriptedInfo::isTrue(asset, DEF_MIRROR);
        aux->cull = mirror? gl::Back : gl::Front;
        // Assimp's coordinate system uses different handedness than Doomsday,
        // so mirroring is needed.
        aux->transformation = Matrix4f::unnormalizedFrame(front, up, !mirror);
        aux->autoscaleToThingHeight = !ScriptedInfo::isFalse(asset, DEF_AUTOSCALE, false);

        // Custom texture maps.
        if(asset.has(DEF_MATERIAL))
        {
            auto mats = asset.subrecord(DEF_MATERIAL).subrecords();
            DENG2_FOR_EACH_CONST(Record::Subrecords, mat, mats)
            {
                handleMaterialTexture(model, mat.key(), *mat.value(), "diffuseMap",  ModelDrawable::Diffuse);
                handleMaterialTexture(model, mat.key(), *mat.value(), "normalMap",   ModelDrawable::Normals);
                handleMaterialTexture(model, mat.key(), *mat.value(), "heightMap",   ModelDrawable::Height);
                handleMaterialTexture(model, mat.key(), *mat.value(), "specularMap", ModelDrawable::Specular);
                handleMaterialTexture(model, mat.key(), *mat.value(), "emissiveMap", ModelDrawable::Emissive);
            }
        }

        // Set up the animation sequences for states.
        if(asset.has(DEF_ANIMATION))
        {
            auto states = ScriptedInfo::subrecordsOfType("state", asset.subrecord(DEF_ANIMATION));
            DENG2_FOR_EACH_CONST(Record::Subrecords, state, states)
            {
                // Note that the sequences are added in alphabetical order.
                auto seqs = ScriptedInfo::subrecordsOfType("sequence", *state.value());
                DENG2_FOR_EACH_CONST(Record::Subrecords, seq, seqs)
                {
                    aux->animations[state.key()] << AnimSequence(seq.key(), *seq.value());
                }
            }

            // TODO: Check for a possible timeline and calculate time factors accordingly.
        }

        // If no rendering passes were defined, specify one default pass.
        if(aux->passes.isEmpty())
        {
            Pass pass;
            pass.meshes.resize(model.meshCount());
            pass.meshes.fill(true);
            aux->passes.append(pass);
        }

        // Store the additional information in the bank.
        bank.setUserData(path, aux.release());
    }

    void handleMaterialTexture(ModelDrawable &model,
                               String const &matName,
                               Record const &matDef,
                               String const &textureName,
                               ModelDrawable::TextureMap map)
    {
        if(matDef.has(textureName))
        {
            String path = ScriptedInfo::absolutePathInContext(matDef, matDef.gets(textureName));

            int matId = identifierFromText(matName, [&model] (String const &text) {
                return model.materialId(text);
            });

            model.setTexturePath(matId, map, path);
        }
    }

    void setupLighting(VisEntityLighting const &lighting)
    {
        // Ambient color and lighting vectors.
        setAmbientLight(lighting.ambientColor * .8f);
        clearLights();
        ClientApp::renderSystem().forAllVectorLights(lighting.vLightListIdx,
                                                     [this] (VectorLightData const &vlight)
        {
            // Use this when drawing the model.
            addLight(vlight.direction.xzy(), vlight.color);
            return LoopContinue;
        });
    }

    /**
     * Sets up the transformation matrices.
     *
     * @param relativeEyePos  Position of the eye in relation to object (in world space).
     * @param modelToLocal    Transformation from model space to the object's local space
     *                        (object's local frame in world space).
     * @param localToView     Transformation from local space to projected view space.
     */
    void setTransformation(Vector3f const &relativeEyePos,
                           Matrix4f const &modelToLocal,
                           Matrix4f const &localToView)
    {
        uMvpMatrix   = localToView * modelToLocal;
        inverseLocal = modelToLocal.inverse();
        uEyePos      = inverseLocal * relativeEyePos;
    }

    /**
     * Sets up the transformation matrices for an eye-space view. The eye position is
     * at (0, 0, 0).
     *
     * @param modelToLocal  Transformation from model space to the object's local space
     *                      (object's local frame in world space).
     * @param inverseLocal  Transformation from local space to model space, taking
     *                      the object's rotation in world space into account.
     * @param localToView   Transformation from local space to projected view space.
     */
    void setEyeSpaceTransformation(Matrix4f const &modelToLocal,
                                   Matrix4f const &inverseLocalMat,
                                   Matrix4f const &localToView)
    {
        uMvpMatrix   = localToView * modelToLocal;
        inverseLocal = inverseLocalMat;
        uEyePos      = inverseLocal * Vector3f();
    }

    void setAmbientLight(Vector3f const &ambientIntensity)
    {
        uAmbientLight = Vector4f(ambientIntensity, 1.f);
    }

    void clearLights()
    {
        lightCount = 0;

        for(int i = 0; i < MAX_LIGHTS; ++i)
        {
            uLightDirs       .set(i, Vector3f());
            uLightIntensities.set(i, Vector4f());
        }
    }

    void addLight(Vector3f const &direction, Vector3f const &intensity)
    {
        if(lightCount == MAX_LIGHTS) return;

        int idx = lightCount;
        uLightDirs       .set(idx, (inverseLocal * direction).normalize());
        uLightIntensities.set(idx, Vector4f(intensity, intensity.max()));

        lightCount++;
    }

    void draw(ModelDrawable const &model,
              ModelDrawable::Animator const &animator,
              AuxiliaryData const &auxData)
    {
        for(Pass const &pass : auxData.passes)
        {
            GLState::push()
                    .setBlendFunc(pass.blendFunc)
                    .setBlendOp(pass.blendOp);

            // Draw the model using the current animation state.
            model.draw(&animator, &pass.meshes);

            GLState::pop();
        }
    }
};

ModelRenderer::ModelRenderer() : d(new Instance(this))
{}

void ModelRenderer::glInit()
{
    d->init();
}

void ModelRenderer::glDeinit()
{
    d->deinit();
}

ModelBank &ModelRenderer::bank()
{
    return d->bank;
}

ModelRenderer::StateAnims const *ModelRenderer::animations(DotPath const &modelId) const
{
    if(auto const *aux = d->bank.userData(modelId)->maybeAs<AuxiliaryData>())
    {
        if(!aux->animations.isEmpty())
        {
            return &aux->animations;
        }
    }
    return 0;
}

void ModelRenderer::render(vissprite_t const &spr)
{
    /*
     * Work in progress:
     *
     * Here is the contact point between the old renderer and the new GL2 model renderer.
     * In the future, vissprites should form a class hierarchy, and the entire drawing
     * operation should be encapsulated within. This will allow drawing a model (or a
     * sprite, etc.) by creating a VisSprite instance and telling it to draw itself.
     */

    drawmodel2params_t const &p = spr.data.model2;
    gl::Cull culling = gl::Back;

    Vector3d const modelWorldOrigin = (spr.pose.origin + spr.pose.srvo).xzy();

    Matrix4f modelToLocal =
            Matrix4f::rotate(-90 + (spr.pose.viewAligned? spr.pose.yawAngleOffset :
                                                          spr.pose.yaw),
                             Vector3f(0, 1, 0) /* vertical axis for yaw */);
    Matrix4f localToView =
            Viewer_Matrix() *
            Matrix4f::translate(modelWorldOrigin) *
            Matrix4f::scale(Vector3f(1.0f, 1.0f/1.2f, 1.0f)); // Inverse aspect correction.

    if(p.object)
    {
        auto const &mobjData = THINKER_DATA(p.object->thinker, ClientMobjThinkerData);
        modelToLocal = modelToLocal * mobjData.modelTransformation();
        culling = mobjData.auxiliaryModelData().cull;
    }

    GLState::push().setCull(culling);

    // Set up a suitable matrix for the pose.
    d->setTransformation(Rend_EyeOrigin() - modelWorldOrigin, modelToLocal, localToView);

    // Ambient color and lighting vectors.
    d->setupLighting(spr.light);

    // Draw the model using the current animation state.
    d->draw(*p.model, *p.animator, *p.auxData);

    GLState::pop();

    /// @todo Something is interfering with the cull setting elsewhere (remove this).
    GLState::current().setCull(gl::Back).apply();
}

void ModelRenderer::render(vispsprite_t const &pspr)
{
    auto const &p = pspr.data.model2;

    Matrix4f modelToLocal =
            Matrix4f::rotate(180, Vector3f(0, 1, 0)) *
            p.auxData->transformation;

    Matrix4f localToView = GL_GetProjectionMatrix() * Matrix4f::translate(Vector3f(0, -10, 11));
    d->setEyeSpaceTransformation(modelToLocal,
                                 modelToLocal.inverse() *
                                 Matrix4f::rotate(vpitch, Vector3f(1, 0, 0)) *
                                 Matrix4f::rotate(vang,   Vector3f(0, 1, 0)),
                                 localToView);

    d->setupLighting(pspr.light);

    GLState::push().setCull(p.auxData->cull);
    d->draw(*p.model, *p.animator, *p.auxData);
    GLState::pop();

    /// @todo Something is interfering with the cull setting elsewhere (remove this).
    GLState::current().setCull(gl::Back).apply();
}

int ModelRenderer::identifierFromText(String const &text,
                                      std::function<int (String const &)> resolver) // static
{
    /// @todo This might be useful on a more general level, outside ModelRenderer. -jk

    int id = 0;
    if(text.beginsWith('@'))
    {
        id = text.mid(1).toInt();
    }
    else
    {
        id = resolver(text);
    }
    return id;
}
