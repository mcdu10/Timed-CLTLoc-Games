//
// Created by Maria Carmen D'urbano on 27/08/25.
//

#ifndef IMAGE_H
#define IMAGE_H

#include "RegionTransitionSystem.h"
#include "Region.h"
#include <string>
#include "TimedArena.h"

void exportRTSGraphSmart(const RTS& rts, const TimedArena::TAr arena, const std::string& dotFile, const std::string& logic);
void generateRTSImageSmart(const std::string& dotFile, const std::string& imageFile);
void openRTSGraphSVG(const std::string& dotFile, const std::string& svgFile);
std::string buildClockExpression(
    const std::map<std::string,int>& floorValues,
    const std::set<std::string>& zeroFraction,
    const std::vector<std::vector<std::string>>& fractionalOrder);
#endif //IMAGE_H
