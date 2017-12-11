#include "engine/internal_route_result.hpp"
#include "engine/phantom_node.hpp"

#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

BOOST_AUTO_TEST_SUITE(collapse_test)

using namespace osrm;
using namespace osrm::util;
using namespace osrm::engine;

BOOST_AUTO_TEST_CASE(unchanged_collapse_route_result)
{
    PhantomNode source;
    PhantomNode target;
    source.forward_segment_id = {1, true};
    target.forward_segment_id = {6, true};
    PathData pathy{2, 17, false, 2, 3, 4, 5, 0, {}, 4, 2, {}, 2, {1.0}, {1.0}, false};
    PathData kathy{1, 16, false, 1, 2, 3, 4, 1, {}, 3, 1, {}, 1, {2.0}, {3.0}, false};
    InternalRouteResult one_leg_result{std::vector<std::vector<PathData>>{std::vector<PathData>{pathy, kathy}},
        std::vector<PhantomNodes>{PhantomNodes{source, target}},
        std::vector<bool>{true},
        std::vector<bool>{true},
        50};

    auto collapsed = CollapseInternalRouteResult(one_leg_result, {true, true});
    BOOST_CHECK_EQUAL(one_leg_result.unpacked_path_segments[0].front().turn_via_node, collapsed.unpacked_path_segments[0].front().turn_via_node);
    BOOST_CHECK_EQUAL(one_leg_result.unpacked_path_segments[0].back().turn_via_node, collapsed.unpacked_path_segments[0].back().turn_via_node);
}

BOOST_AUTO_TEST_CASE(two_legs_to_one_leg)
{
    PathData pathy{2, 17, false, 2, 3, 4, 5, 0, {}, 4, 2, {}, 2, {1.0}, {1.0}, false};
    PathData kathy{1, 16, false, 1, 2, 3, 4, 1, {}, 3, 1, {}, 1, {2.0}, {3.0}, false};
    PathData cathy{3, 16, false, 1, 2, 3, 4, 1, {}, 3, 1, {}, 1, {2.0}, {3.0}, false};
    PhantomNode node_1;
    PhantomNode node_2;
    PhantomNode node_3;
    node_1.forward_segment_id = {1, true};
    node_2.forward_segment_id = {6, true};
    node_3.forward_segment_id = {12, true};
    InternalRouteResult three_leg_result{std::vector<std::vector<PathData>>{std::vector<PathData>{pathy, kathy}, std::vector<PathData>{kathy, cathy}},
        std::vector<PhantomNodes>{PhantomNodes{node_1, node_2}, PhantomNodes{node_2, node_3}},
        std::vector<bool>{true, false},
        std::vector<bool>{true, false},
        80};

    auto collapsed = CollapseInternalRouteResult(three_leg_result, {true, false, true, true});
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments.size(), 1);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates.size(), 1);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[0].target_phantom.forward_segment_id.id, 12);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[0].source_phantom.forward_segment_id.id, 1);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0].size(), 3);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0][0].turn_via_node, 2);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0][1].turn_via_node, 1);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0][2].turn_via_node, 3);
}

BOOST_AUTO_TEST_CASE(three_legs_to_two_legs)
{
    PathData pathy{2, 17, false, 2, 3, 4, 5, 0, {}, 4, 2, {}, 2, {1.0}, {1.0}, false};
    PathData kathy{1, 16, false, 1, 2, 3, 4, 1, {}, 3, 1, {}, 1, {2.0}, {3.0}, false};
    PathData qathy{5, 16, false, 1, 2, 3, 4, 1, {}, 3, 1, {}, 1, {2.0}, {3.0}, false};
    PathData cathy{3, 16, false, 1, 2, 3, 4, 1, {}, 3, 1, {}, 1, {2.0}, {3.0}, false};
    PathData mathy{4, 18, false, 8, 9, 13, 4, 2, {}, 4, 2, {}, 2, {3.0}, {1.0}, false};
    PhantomNode node_1;
    PhantomNode node_2;
    PhantomNode node_3;
    PhantomNode node_4;
    node_1.forward_segment_id = {1, true};
    node_2.forward_segment_id = {6, true};
    node_3.forward_segment_id = {12, true};
    node_4.forward_segment_id = {18, true};
    InternalRouteResult three_leg_result{std::vector<std::vector<PathData>>{std::vector<PathData>{pathy, kathy}, std::vector<PathData>{kathy, qathy, cathy}, std::vector<PathData>{cathy, mathy}},
        std::vector<PhantomNodes>{PhantomNodes{node_1, node_2}, PhantomNodes{node_2, node_3}, PhantomNodes{node_3, node_4}},
        std::vector<bool>{true, false, true},
        std::vector<bool>{true, false, true},
        140};

    auto collapsed = CollapseInternalRouteResult(three_leg_result, {true, true, false, true});
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments.size(), 2);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates.size(), 2);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[0].source_phantom.forward_segment_id.id, 1);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[0].target_phantom.forward_segment_id.id, 6);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[1].source_phantom.forward_segment_id.id, 6);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[1].target_phantom.forward_segment_id.id, 18);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0].size(), 2);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[1].size(), 4);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0][0].turn_via_node, 2);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0][1].turn_via_node, 1);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[1][0].turn_via_node, 1);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[1][1].turn_via_node, 5);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[1][2].turn_via_node, 3);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[1][3].turn_via_node, 4);
}

BOOST_AUTO_TEST_CASE(two_legs_to_two_legs)
{
    PathData pathy{2, 17, false, 2, 3, 4, 5, 0, {}, 4, 2, {}, 2, {1.0}, {1.0}, false};
    PathData kathy{1, 16, false, 1, 2, 3, 4, 1, {}, 3, 1, {}, 1, {2.0}, {3.0}, false};
    PathData cathy{3, 16, false, 1, 2, 3, 4, 1, {}, 3, 1, {}, 1, {2.0}, {3.0}, false};
    PhantomNode node_1;
    PhantomNode node_2;
    PhantomNode node_3;
    node_1.forward_segment_id = {1, true};
    node_2.forward_segment_id = {6, true};
    node_3.forward_segment_id = {12, true};
    InternalRouteResult three_leg_result{std::vector<std::vector<PathData>>{std::vector<PathData>{pathy, kathy}, std::vector<PathData>{kathy, cathy}},
        std::vector<PhantomNodes>{PhantomNodes{node_1, node_2}, PhantomNodes{node_2, node_3}},
        std::vector<bool>{true, false},
        std::vector<bool>{true, false},
        80};

    auto collapsed = CollapseInternalRouteResult(three_leg_result, {true, true, true});
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments.size(), 2);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates.size(), 2);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[0].source_phantom.forward_segment_id.id, 1);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[0].target_phantom.forward_segment_id.id, 6);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[1].source_phantom.forward_segment_id.id, 6);
    BOOST_CHECK_EQUAL(collapsed.segment_end_coordinates[1].target_phantom.forward_segment_id.id, 12);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0].size(), 2);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[1].size(), 2);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0][0].turn_via_node, 2);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[0][1].turn_via_node, 1);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[1][0].turn_via_node, 1);
    BOOST_CHECK_EQUAL(collapsed.unpacked_path_segments[1][1].turn_via_node, 3);
}

BOOST_AUTO_TEST_SUITE_END()
