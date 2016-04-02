#pragma once

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace pt = boost::property_tree;

pt::ptree loadPtree(const std::string& fileName);
void savePtree(const std::string& fileName, const pt::ptree& ptree);
void testPrintPtree(pt::ptree& tree, int level = 0);
