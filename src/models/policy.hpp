#ifndef POLICY_H
#define POLICY_H

#include <algorithm>                 // for std::for_each
#include <iostream>                  // for std::cout
#include <string>
#include <unordered_map>
#include <utility>                   // for std::pair
#include <vector>

#include "rule.hpp"
#include "hierarchy.hpp"
#include "./../utils/vectors.hpp"

/* Order is important ! */
#include <boost/graph/transitive_closure.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>



using namespace boost;
using namespace std;

class Policy {

typedef bimap <int, std::string> bm_type;
typedef std::unordered_map <int, Rule> um_type;
typedef property <vertex_name_t, char> Name;
typedef property <vertex_index_t, std::size_t, Name> Index;
typedef adjacency_list <listS, listS, directedS, Index> Graph;
typedef graph_traits <Graph>::vertex_descriptor vertex_t;

public:
  Policy() {}

  void addActorVertices(std::vector<std::string> actorsVector) {
    for(unsigned int i = 0; i < actorsVector.size(); i++) {
      actors.addVertex(actorsVector[i]);
    }
  }

  void addObjectVertices(std::vector<std::string> objectsVector) {
    for(unsigned int i = 0; i < objectsVector.size(); i++) {
      objects.addVertex(objectsVector[i]);
    }
  }

  void addActorsEdge(int i, int j) {
    actors.addEdge(i, j);
  }

  void addObjectsEdge(int i, int j) {
    objects.addEdge(i, j);
  }

  void addActorsEdges(std::vector< pair<int, int> > v) {
    int l = v.size();
    for(int i=0; i<l; i++) {
      actors.addEdge(v[i].first, v[i].second);
    }
  }

  void addObjectsEdges(std::vector< pair<int, int> > v) {
    int l = v.size();
    for(int i=0; i<l; i++) {
      objects.addEdge(v[i].first, v[i].second);
    }
  }

  void addRule(Rule& r) {
    rulesM.insert(std::pair<int, Rule>(r.id, r));
  }

  void addRules(std::vector<Rule> v) {
    int l = v.size();
    for(int i=0; i<l; i++) {
      addRule(v[i]);
    }
  }

  std::vector<int> effectiveRules(int actor, int object) {
    bm_type::left_const_iterator aIt = actors.namesBimap.left.find(actor);
    bm_type::left_const_iterator oIt = objects.namesBimap.left.find(object);
    std::cout << "Demande de " << aIt->second << " sur " << oIt->second << std::endl;
    std::vector<int> result, adjacent = actors.inAdjacentIndexVertices(actor);
    cout << "adjacent " << adjacent << endl;
    int m = adjacent.size();

    for(um_type::iterator it = rulesM.begin(); it != rulesM.end(); it++) {
      if(it->second.actor == actor) {
        result.push_back(it->second.id);
        continue;
      }
      for(int j=0; j<m; j++) {
        if(adjacent[j] == it->second.actor) {
          result.push_back(it->second.id);
        }
      }
    }
    return result;
  }



  std::vector<int> deepestRules(std::vector <int> rules, int actor) {
    bm_type::left_const_iterator aIt = actors.namesBimap.left.find(actor);
    std::unordered_map<int, Rule>::const_iterator rIt;
    std::unordered_map<int, std::vector<int> >::iterator it, it0;
    std::cout << "Actor " << aIt->second << "(" << aIt->first << ")" << std::endl;

    std::vector <int> result;
    int l = rules.size();
    if(l<=1) {
      return rules;
    }
    // If there is at least one rule on this actor
    for(int i=0; i<l; i++) {
      /* rIt ne devrait pas être nul */
      rIt = rulesM.find(rules[i]);
      if(rIt != rulesM.end()) {
        // cout << rIt->first << "->" << rIt->second << endl;
        if(rIt->second.actor == actor) {
          result.push_back(rules[i]);
        }
      }
    }

    if(result.size() > 0) {
      return result;
    }

    // Liste des acteurs ancêtres à actors :
    std::vector<int> adjacents = actors.inAdjacentIndexVertices(actor);
    std::cout << "adjacentIndexVertices" << adjacents << std::endl;

    std::vector<int> inRange;
    std::unordered_map<int, std::vector<int> > inRangeAdjacentsUmap;

    for(int i=0; i<l; i++) {
      if(vectorContains(adjacents, rules[i])) {
        inRange.push_back(rules[i]);
        inRangeAdjacentsUmap.insert(pair<int, std::vector<int> >
          (rules[i], actors.inAdjacentIndexVertices(rules[i])));
      }
    }
    cout << "inRange " << inRange << endl;
    /* Suppression de tous les moins spécifiques !! */
    std::vector<int> tmp;
    std::vector<int> toRemove;
    for(it = inRangeAdjacentsUmap.begin(); it != inRangeAdjacentsUmap.end(); it++) {
      cout << it->first << ": adjacents=";
      tmp = it->second;
      for(unsigned int i = 0; i < tmp.size(); i++) {
        if(inRangeAdjacentsUmap.find(tmp[i]) != inRangeAdjacentsUmap.end()) {
          // remove de inrange
          toRemove.push_back(tmp[i]);
        }
        cout << tmp[i] << ",";
      }
      cout << endl;
    }

    result = vectorDiff(inRange, toRemove);
    return result;
  }

  bool sumModalities(std::vector<int> rulesId) {
    int l = rulesId.size();
    for(int i=0; i<l; i++) {
      if(!rulesM.find(rulesId[i])->second.permission) {
        return false;
      }
    }
    return true;
  }


  Hierarchy actors;
  Hierarchy objects;
  std::unordered_map<int, Rule> rulesM;

private:
};

#endif
