/*  BOSS

    A plugin load order optimiser for games that use the esp/esm plugin system.

    Copyright (C) 2012    WrinklyNinja

    This file is part of BOSS.

    BOSS is free software: you can redistribute
    it and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    BOSS is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BOSS.  If not, see
    <http://www.gnu.org/licenses/>.
*/

#include "graph.h"
#include "streams.h"

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>

using namespace std;

namespace boss {
    vertex_t GetPluginVertex(PluginGraph& graph, const Plugin& plugin, boost::unordered_map<std::string, vertex_t>& pluginVertexMap) {

        vertex_t vertex;
        string name = boost::to_lower_copy(plugin.Name());

        boost::unordered_map<string, vertex_t>::iterator vertexMapIt = pluginVertexMap.find(name);
        if (vertexMapIt == pluginVertexMap.end()) {
            BOOST_LOG_TRIVIAL(trace) << "Vertex for \"" << name << "\" doesn't exist, creating one.";
            Plugin p;
           // vertex = boost::add_vertex(p, graph);
            BOOST_LOG_TRIVIAL(trace) << "Adding vertex to map.";
            pluginVertexMap.emplace(name, vertex);
        } else
            vertex = vertexMapIt->second;

        return vertex;
    }

    //The map maps each plugin name to a vector of names of plugins that overlap with it and should load before it.
    void CalcPluginOverlaps(const std::list<Plugin>& plugins, boost::unordered_map< std::string, std::vector<std::string> >& overlapMap) {
        for (list<Plugin>::const_iterator it=plugins.begin(),
                                          endit=plugins.end();
                                          it != endit;
                                          ++it) {
            list<Plugin>::const_iterator jt = it;
            ++jt;
            for (jt, endit; jt != endit; ++jt) {
                    BOOST_LOG_TRIVIAL(trace) << "Checking for FormID overlap between \"" << it->Name() << "\" and \"" << jt->Name() << "\".";
                if (it->DoFormIDsOverlap(*jt)) {
                    std::string key;
                    std::string value;
                    //Priority values should override the number of override records as the deciding factor if they differ.
                    if (it->MustLoadAfter(*jt) || jt->MustLoadAfter(*it))
                        break;
                    if (it->Priority() < jt->Priority()) {
                        key = jt->Name();
                        value = it->Name();
                    } else if (jt->Priority() < it->Priority()) {
                        key = it->Name();
                        value = jt->Name();
                    } else if (it->NumOverrideFormIDs() >= jt->NumOverrideFormIDs()) {
                        key = jt->Name();
                        value = it->Name();
                    } else {
                        key = it->Name();
                        value = jt->Name();
                    }
                    boost::unordered_map< string, vector<string> >::iterator mapIt = overlapMap.find(key);
                    if (mapIt == overlapMap.end()) {
                        overlapMap.insert(pair<string, vector<string> >(key, vector<string>(1, value)));
                    } else {
                        mapIt->second.push_back(value);
                    }
                }
            }
        }
    }

    bool GetVertexByName(const PluginGraph& graph, const std::string& name, vertex_t& vertex) {
        vertex_it vit, vit_end;
        boost::tie(vit, vit_end) = boost::vertices(graph);

        for (vit, vit_end; vit != vit_end; ++vit) {
            if (boost::iequals(graph[*vit]->Name(), name)) {
                vertex = *vit;
                return true;
            }
        }

        return false;
    }

    void SaveGraph(const PluginGraph& graph, const boost::filesystem::path outpath) {
        //First need to extract vertex names, since their stored as private members otherwise.
  /*      vector<string> names;
        vertex_it vit, vit_end;
        boost::tie(vit, vit_end) = boost::vertices(graph);

        for (vit, vit_end; vit != vit_end; ++vit) {
            names.push_back(graph[*vit]->Name());
        }
        //Now write graph to file.
        boss::ofstream out(outpath);
        boost::write_graphviz(out, graph, boost::make_label_writer(&names[0]));
        out.close();
 */   }

    void Sort(const PluginGraph& graph, std::list<Plugin>& plugins) {
 /*       std::list<vertex_t> sortedVertices;
        boost::topological_sort(graph, std::front_inserter(sortedVertices));

        BOOST_LOG_TRIVIAL(info) << "Calculated order: ";
        list<boss::Plugin> tempPlugins;
        for (std::list<vertex_t>::iterator it = sortedVertices.begin(), endit = sortedVertices.end(); it != endit; ++it) {
            BOOST_LOG_TRIVIAL(info) << '\t' << graph[*it]->Name();
            tempPlugins.push_back(*graph[*it]);
        }
        plugins.swap(tempPlugins);
 */   }
}