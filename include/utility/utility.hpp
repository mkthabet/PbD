/*
 * utility.hpp
 *
 *  Created on: May 25, 2015
 *      Author: M. Thabet
 */

#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <boost/array.hpp>

#include "geometry_msgs/Pose.h"
#include "sensor_msgs/JointState.h"

namespace iros {
namespace pbd {
namespace segmentation {

double getDistance(boost::shared_ptr<geometry_msgs::Pose> p1,
		boost::shared_ptr<geometry_msgs::Pose> p2) {
	return sqrt(
			(p1->position.x - p2->position.x)
					* (p1->position.x - p2->position.x)
					+ (p1->position.y - p2->position.y)
							* (p1->position.y - p2->position.y)
					+ (p1->position.z - p2->position.z)
							* (p1->position.z - p2->position.z));
}

} /* namsespace primitives */
} /* namsespace pbd */
} /* namsespace iros */

#endif /* UTILITY_HPP_ */
