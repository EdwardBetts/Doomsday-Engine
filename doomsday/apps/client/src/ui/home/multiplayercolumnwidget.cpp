/** @file multiplayercolumnwidget.cpp
 *
 * @authors Copyright (c) 2016 Jaakko Keränen <jaakko.keranen@iki.fi>
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

#include "ui/home/multiplayercolumnwidget.h"
#include "ui/home/multiplayerpanelbuttonwidget.h"
#include "ui/widgets/homemenuwidget.h"
#include "ui/home/headerwidget.h"
#include "network/serverlink.h"
#include "clientapp.h"

#include <doomsday/doomsdayapp.h>
#include <doomsday/games.h>
#include <de/MenuWidget>

using namespace de;

DENG_GUI_PIMPL(MultiplayerColumnWidget)
, DENG2_OBSERVES(DoomsdayApp, GameChange)
, DENG2_OBSERVES(Games, Readiness)
, DENG2_OBSERVES(ServerLink, DiscoveryUpdate)
, public ChildWidgetOrganizer::IWidgetFactory
{
    static ServerLink &link() { return ClientApp::serverLink(); }
    static String hostId(serverinfo_t const &sv) {
        return String("%1:%2").arg(sv.address).arg(sv.port);
    }

    /**
     * Data item with information about a found server.
     */
    class ServerListItem : public ui::Item
    {
    public:
        ServerListItem(serverinfo_t const &serverInfo)
        {
            setData(hostId(serverInfo));
            _info = serverInfo;
        }

        serverinfo_t const &info() const
        {
            return _info;
        }

        void setInfo(serverinfo_t const &serverInfo)
        {
            _info = serverInfo;
            notifyChange();
        }

        String title() const
        {
            return _info.name;
        }

        String gameId() const
        {
            return _info.gameIdentityKey;
        }

    private:
        serverinfo_t _info;
    };

//- Private state -------------------------------------------------------------

    DiscoveryMode mode = NoDiscovery;
    ServerLink::FoundMask mask = ServerLink::Any;
    HomeMenuWidget *menu;

    Instance(Public *i) : Base(i)
    {
        link().audienceForDiscoveryUpdate += this;
        DoomsdayApp::app().audienceForGameChange() += this;
        DoomsdayApp::games().audienceForReadiness() += this;

        // Set up the widgets.
        ScrollAreaWidget &area = self.scrollArea();
        area.add(menu = new HomeMenuWidget);
        menu->organizer().setWidgetFactory(*this);

        menu->rule()
                .setInput(Rule::Width, area.contentRule().width())
                .setInput(Rule::Left,  area.contentRule().left())
                .setInput(Rule::Top,   self.header().rule().bottom()/* +
                                       rule("gap")*/);
    }

    ~Instance()
    {
        link().audienceForDiscoveryUpdate -= this;
        DoomsdayApp::app().audienceForGameChange() -= this;
        DoomsdayApp::games().audienceForReadiness() -= this;
    }

    void linkDiscoveryUpdate(ServerLink const &link)
    {
        bool changed = false;

        ui::Data &items = menu->items();

        // Remove obsolete entries.
        for (ui::Data::Pos idx = 0; idx < items.size(); ++idx)
        {
            String const id = items.at(idx).data().toString();
            if (!link.isFound(Address::parse(id), mask))
            {
                items.remove(idx--);
                changed = true;
            }
        }

        // Add new entries and update existing ones.
        foreach (de::Address const &host, link.foundServers(mask))
        {
            serverinfo_t info;
            if (!link.foundServerInfo(host, &info, mask)) continue;

            ui::Data::Pos found = items.findData(hostId(info));
            if (found == ui::Data::InvalidPos)
            {
                // Needs to be added.
                items.append(new ServerListItem(info));
                changed = true;
            }
            else
            {
                // Update the info.
                items.at(found).as<ServerListItem>().setInfo(info);
            }
        }

        if (changed)
        {
            items.sort([] (ui::Item const &a, ui::Item const &b)
            {
                auto const &first  = a.as<ServerListItem>();
                auto const &second = b.as<ServerListItem>();

                // Primarily sort by number of players.
                if (first.info().numPlayers == second.info().numPlayers)
                {
                    // Secondarily by game ID.
                    int cmp = qstrcmp(first .info().gameIdentityKey,
                                      second.info().gameIdentityKey);
                    if (!cmp)
                    {
                        // Lastly by server name.
                        return qstricmp(first.info().name,
                                        second.info().name) < 0;
                    }
                    return cmp < 0;
                }
                return first.info().numPlayers - second.info().numPlayers > 0;
            });
        }
    }

    void currentGameChanged(Game const &newGame)
    {
        if (newGame.isNull() && mode == DiscoverUsingMaster)
        {
            // If the session menu exists across game changes, it's good to
            // keep it up to date.
            link().discoverUsingMaster();
        }
    }

    void gameReadinessUpdated()
    {
        foreach (Widget *w, menu->childWidgets())
        {
            updateAvailability(w->as<GuiWidget>());
        }
    }

//- ChildWidgetOrganizer::IWidgetFactory --------------------------------------

    GuiWidget *makeItemWidget(ui::Item const &, GuiWidget const *)
    {
        return new MultiplayerPanelButtonWidget;
    }

    void updateItemWidget(GuiWidget &widget, ui::Item const &item)
    {
        auto const &serverItem = item.as<ServerListItem>();

        widget.as<MultiplayerPanelButtonWidget>()
                .updateContent(serverItem.info());

        // Is it playable?
        updateAvailability(widget);
    }

    void updateAvailability(GuiWidget &menuItemWidget)
    {
        auto const &item = menu->organizer().findItemForWidget(menuItemWidget)->as<ServerListItem>();

        bool playable = false;
        String gameId = item.gameId();
        if (DoomsdayApp::games().contains(gameId))
        {
            playable = DoomsdayApp::games()[gameId].isPlayable();
        }
        menuItemWidget.enable(playable);
    }
};

MultiplayerColumnWidget::MultiplayerColumnWidget()
    : ColumnWidget("multiplayer-column")
    , d(new Instance(this))
{
    d->mode = DiscoverUsingMaster;
    d->link().discoverUsingMaster();

    scrollArea().setContentSize(maximumContentWidth(),
                                header().rule().height() +
                                rule("gap") +
                                d->menu->rule().height());

    header().title().setText(_E(s) "dengine.net\n" _E(.)_E(w) + tr("Multiplayer Games"));
    header().info().setText(tr("Multiplayer servers are discovered via the dengine.net master server and broadcasting on the local network."));
}

String MultiplayerColumnWidget::tabHeading() const
{
    return tr("Multiplayer");
}

String MultiplayerColumnWidget::configVariableName() const
{
    return "home.columns.multiplayer";
}

void MultiplayerColumnWidget::setHighlighted(bool highlighted)
{
    ColumnWidget::setHighlighted(highlighted);

    if (highlighted)
    {
        d->menu->restorePreviousSelection();
    }
    else
    {
        root().setFocus(nullptr);
        d->menu->unselectAll();
    }
}

#if 0
DENG_GUI_PIMPL(MPSessionMenuWidget)
, DENG2_OBSERVES(DoomsdayApp, GameChange)
, DENG2_OBSERVES(Games, Readiness)
, DENG2_OBSERVES(ServerLink, DiscoveryUpdate)
{
    /**
     * Action for joining a game on a multiplayer server.
     */
    class JoinAction : public de::Action
    {
        String gameId;
        String cmd;

    public:
        JoinAction(serverinfo_t const &sv)
        {
            gameId = sv.gameIdentityKey;
            cmd    = String("connect %1 %2").arg(sv.address).arg(sv.port);
        }

        void trigger()
        {
            Action::trigger();

            BusyMode_FreezeGameForBusyMode();
            ClientWindow::main().taskBar().close();

            // Automatically leave the current MP game.
            if (netGame && isClient)
            {
                ClientApp::serverLink().disconnect();
            }

            DoomsdayApp::app().changeGame(DoomsdayApp::games()[gameId], DD_ActivateGameWorker);
            Con_Execute(CMDS_DDAY, cmd.toLatin1(), false, false);
        }
    };

    /**
     * Data item with information about a found server.
     */
    class ServerListItem : public ui::Item, public SessionItem
    {
    public:
        ServerListItem(serverinfo_t const &serverInfo, SessionMenuWidget &owner)
            : SessionItem(owner)
        {
            setData(hostId(serverInfo));
            _info = serverInfo;
        }

        serverinfo_t const &info() const
        {
            return _info;
        }

        void setInfo(serverinfo_t const &serverInfo)
        {
            _info = serverInfo;
            notifyChange();
        }

        String title() const
        {
            return _info.name;
        }

        String gameId() const
        {
            return _info.gameIdentityKey;
        }

    private:
        serverinfo_t _info;
    };

    /**
     * Widget representing a ServerListItem in the dialog's menu.
     */
    struct ServerWidget : public GameSessionWidget
    {
        ServerListItem const *svItem = nullptr;

        ServerWidget()
        {
            loadButton().disable();
        }

        Game const *game() const
        {
            if (!svItem) return nullptr;
            return &App_Games()[svItem->info().gameIdentityKey];
        }

        void updateFromItem(ServerListItem const &item)
        {
            try
            {
                svItem = &item;
                Game const &svGame = *game();

                if (style().images().has(svGame.logoImageId()))
                {
                    loadButton().setImage(style().images().image(svGame.logoImageId()));
                }

                serverinfo_t const &sv = item.info();
                loadButton().setText(String(_E(F)_E(s) "%2\n" _E(.)_E(.)
                                            _E(1) "%1" _E(.)_E(C) "%4" _E(.)_E(D)_E(l) "\n%5 %3")
                               .arg(sv.name)
                               .arg(svGame.title())
                               .arg(sv.gameConfig)
                               .arg(sv.numPlayers? QString(" " DENG2_CHAR_MDASH " %1").arg(sv.numPlayers) : QString())
                               .arg(sv.map));

                // Extra information.
                document().setText(ServerInfo_AsStyledText(&sv));

                updateAvailability();
            }
            catch (Error const &)
            {
                svItem = nullptr;

                /// @todo
            }
        }

        void updateAvailability()
        {
            loadButton().enable(svItem &&
                                svItem->info().canJoin &&
                                svItem->info().version == DOOMSDAY_VERSION &&
                                game()->allStartupFilesFound());
        }
    };

    DiscoveryMode mode;
    ServerLink::FoundMask mask;
    IndirectRule *maxHeightRule = new IndirectRule;

    Instance(Public *i)
        : Base(i)
        , mask(ServerLink::Any)
    {
        link().audienceForDiscoveryUpdate += this;
        DoomsdayApp::app().audienceForGameChange() += this;
        App_Games().audienceForReadiness() += this;
    }

    ~Instance()
    {
        releaseRef(maxHeightRule);
        link().audienceForDiscoveryUpdate -= this;
        DoomsdayApp::app().audienceForGameChange() -= this;
        App_Games().audienceForReadiness() -= this;
    }

    /**
     * Puts together a rule that determines the tallest load button of those present
     * in the menu. This will be used to size all the buttons uniformly.
     */
    void updateItemMaxHeight()
    {
        // Form a rule that is the maximum of all load button heights.
        Rule const *maxHgt = nullptr;
        foreach (Widget *w, self.childWidgets())
        {
            if (ServerWidget *sw = w->maybeAs<ServerWidget>())
            {
                auto const &itemHeight = sw->loadButton().contentHeight();
                if (!maxHgt)
                {
                    maxHgt = holdRef(itemHeight);
                }
                else
                {
                    changeRef(maxHgt, OperatorRule::maximum(*maxHgt, itemHeight));
                }
            }
        }

        if (maxHgt)
        {
            maxHeightRule->setSource(*maxHgt);
        }
        else
        {
            maxHeightRule->unsetSource();
        }
        releaseRef(maxHgt);
    }

    void linkDiscoveryUpdate(ServerLink const &link)
    {
        bool changed = false;

        // Remove obsolete entries.
        for (ui::Data::Pos idx = 0; idx < self.items().size(); ++idx)
        {
            String const id = self.items().at(idx).data().toString();
            if (!link.isFound(Address::parse(id), mask))
            {
                self.items().remove(idx--);
                changed = true;
            }
        }

        // Add new entries and update existing ones.
        foreach (de::Address const &host, link.foundServers(mask))
        {
            serverinfo_t info;
            if (!link.foundServerInfo(host, &info, mask)) continue;

            ui::Data::Pos found = self.items().findData(hostId(info));
            if (found == ui::Data::InvalidPos)
            {
                // Needs to be added.
                self.items().append(new ServerListItem(info, self));
                changed = true;
            }
            else
            {
                // Update the info.
                self.items().at(found).as<ServerListItem>().setInfo(info);
            }
        }

        if (changed)
        {
            updateItemMaxHeight();
            self.sort();

            // Let others know that one or more games have appeared or disappeared
            // from the menu.
            emit self.availabilityChanged();
        }
    }

    void currentGameChanged(Game const &newGame)
    {
        if (newGame.isNull() && mode == DiscoverUsingMaster)
        {
            // If the session menu exists across game changes, it's good to
            // keep it up to date.
            link().discoverUsingMaster();
        }
    }

    void gameReadinessUpdated()
    {
        foreach (Widget *w, self.childWidgets())
        {
            if (ServerWidget *sw = w->maybeAs<ServerWidget>())
            {
                sw->updateAvailability();
            }
        }
    }
};

MPSessionMenuWidget::MPSessionMenuWidget(DiscoveryMode discovery)
    : SessionMenuWidget("mp-session-menu"), d(new Instance(this))
{
    d->mode = discovery;

    switch (discovery)
    {
    case DiscoverUsingMaster:
        d->link().discoverUsingMaster();
        break;

    case DirectDiscoveryOnly:
        // Only show servers found via direct connection.
        d->mask = ServerLink::Direct;
        break;

    default:
        break;
    }
}

Action *MPSessionMenuWidget::makeAction(ui::Item const &item)
{
    return new Instance::JoinAction(item.as<Instance::ServerListItem>().info());
}

GuiWidget *MPSessionMenuWidget::makeItemWidget(ui::Item const &, GuiWidget const *)
{
    auto *sw = new Instance::ServerWidget;
    sw->rule().setInput(Rule::Height, *d->maxHeightRule);
    return sw;
}

void MPSessionMenuWidget::updateItemWidget(GuiWidget &widget, ui::Item const &item)
{
    Instance::ServerWidget &sv = widget.as<Instance::ServerWidget>();
    sv.updateFromItem(item.as<Instance::ServerListItem>());
}
#endif
