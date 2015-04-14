//=======================================================================
// Copyright (c) 2015 Romain Cotte. All rights reserved.
//
// Distributed under the GNU Affero General Public License,
// Version 3.0. (See https://www.gnu.org/licenses/agpl-3.0.html)
//=======================================================================

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <unordered_map>
#include <string>

#include "./../models/rule.hpp"
#include "./../models/hierarchy.hpp"
#include "./../models/policy.hpp"
#include "./../utils/vectors.hpp"

/* Order is important ! */
#include <boost/graph/transitive_closure.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/ui/text/TestRunner.h>

using namespace boost;
using namespace std;

class GraphTest : public CppUnit::TestFixture {
typedef bimap< int, std::string> bm_type;

private:
  Policy policy;
  std::vector<Rule> rules;
  std::unordered_map<int, Rule> rulesUmap;
  int actor;
  int object;
public:
  GraphTest() {
  }

  static CppUnit::Test *suite() {
    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("GraphTest");

    suiteOfTests->addTest(new CppUnit::TestCaller<GraphTest>("Test1 - Unique Solution.",
            &GraphTest::test1));
    return suiteOfTests;
  }

  void setUp() {}

  void tearDown() {}

protected:

  void test1() {
    Policy policy;
    std::vector<Rule> rules;
    std::vector<std::string> actorsVector = {
      "CHUS", "Cardiologie", "Urgence", "Doctor B"
    };
    policy.addActorVertices(actorsVector);

    std::vector<std::string> objectsVector = {
      "Lab", "Microbio", "Hemato", "Strep", "Test", "VIH", "Potassium"
    };
    policy.addObjectVertices(objectsVector);

    std::vector< pair<int, int> > actorsEdges = {
      pair<int, int> (0, 1),
      pair<int, int> (0, 2),
      pair<int, int> (1, 3),
      pair<int, int> (2, 3)
    };

    std::vector< pair<int, int> > objectsEdges = {
      pair<int, int> (0, 1),
      pair<int, int> (0, 2),
      pair<int, int> (1, 3),
      pair<int, int> (1, 4),
      pair<int, int> (2, 5),
      pair<int, int> (2, 6)
    };

    policy.addActorsEdges(actorsEdges);
    policy.addObjectsEdges(objectsEdges);

    rules.push_back(Rule(1, 0, 1, 2, false));
    rules.push_back(Rule(2, 0, 2, 2, false));
    rules.push_back(Rule(3, 0, 3, 0, true));
    rules.push_back(Rule(4, 0, 3, 5, false));

    policy.addRules(rules);

    std::vector<int> effective = policy.effectiveRules(3, 5);
    std::vector<int> deepest = policy.deepestRules(effective, 3);
    bool isAllowed = policy.sumModalities(deepest);
    std::cout << "EffectiveRules:" << effective << std::endl;
    std::cout << "DeepestRules:" << deepest << std::endl;
    std::cout << "IsAllowed: " << isAllowed << std::endl;
  }
};


int main(int argc, char* argv[]) {

  CppUnit::TextUi::TestRunner runner;
  std::cout << "Creating Test Suites:" << std::endl;
  runner.addTest(GraphTest::suite());
  std::cout<< "Running the unit tests."<< std::endl;
  runner.run();

  return 0;
}
