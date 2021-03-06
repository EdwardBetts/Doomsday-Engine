/** @file rulebank.cpp  Bank of Rules.
 *
 * @authors Copyright © 2013-2017 Jaakko Keränen <jaakko.keranen@iki.fi>
 *
 * @par License
 * LGPL: http://www.gnu.org/licenses/lgpl.html
 *
 * <small>This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version. This program is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details. You should have received a copy of
 * the GNU Lesser General Public License along with this program; if not, see:
 * http://www.gnu.org/licenses</small>
 */

#include "de/RuleBank"
#include <de/ConstantRule>
#include <de/ScriptedInfo>

namespace de {

DotPath const RuleBank::UNIT("unit");

DENG2_PIMPL_NOREF(RuleBank)
{
    struct RuleSource : public ISource
    {
        InfoBank &bank;
        String id;

        RuleSource(InfoBank &b, String const &ruleId) : bank(b), id(ruleId) {}
        Time modifiedAt() const { return bank.sourceModifiedAt(); }

        Rule *load() const
        {
            Record const &def = bank[id];
            return refless(new ConstantRule(def["constant"].value().asNumber()));
        }
    };

    struct RuleData : public IData
    {
        Rule *rule;

        RuleData(Rule *r) : rule(holdRef(r)) {}
        ~RuleData() { releaseRef(rule); }
    };
};

RuleBank::RuleBank() : InfoBank("RuleBank", DisableHotStorage), d(new Impl)
{}

void RuleBank::addFromInfo(File const &file)
{
    LOG_AS("RuleBank");
    parse(file);
    addFromInfoBlocks("rule");
}

Rule const &RuleBank::rule(DotPath const &path) const
{
    if (path.isEmpty()) return ConstantRule::zero();
    return *static_cast<Impl::RuleData &>(data(path)).rule;
}

Bank::ISource *RuleBank::newSourceFromInfo(String const &id)
{
    return new Impl::RuleSource(*this, id);
}

Bank::IData *RuleBank::loadFromSource(ISource &source)
{
    return new Impl::RuleData(static_cast<Impl::RuleSource &>(source).load());
}

} // namespace de
