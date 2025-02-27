#include "src/HOI4World/Ideas/Ideas.h"
#include "external/common_items/CommonRegexes.h"
#include "external/common_items/Log.h"
#include "external/common_items/ParserHelpers.h"
#include "src/HOI4World/Ideas/IdeaGroup.h"
#include "src/HOI4World/Ideas/IdeaUpdaters.h"
#include <fstream>



HoI4::Ideas::Ideas() noexcept
{
	importIdeologicalIdeas();
	importGeneralIdeas();
}


void HoI4::Ideas::importIdeologicalIdeas()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& ideology, std::istream& theStream) {
		ideologicalIdeas.insert(make_pair(ideology, IdeaGroup(ideology, theStream)));
	});

	parseFile("Configurables/ideologicalIdeas.txt");
	clearRegisteredKeywords();
}


void HoI4::Ideas::importGeneralIdeas()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& ideaGroupName, std::istream& theStream) {
		generalIdeas.emplace_back(IdeaGroup{ideaGroupName, theStream});
	});

	parseFile("Configurables/converterIdeas.txt");
	clearRegisteredKeywords();
}


void HoI4::Ideas::updateIdeas(const std::set<std::string>& majorIdeologies)
{
	Log(LogLevel::Info) << "\tUpdating ideas";

	auto foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup) {
		return (theGroup.getName() == "mobilization_laws");
	});
	updateMobilizationLaws(*foundGroup, majorIdeologies);

	foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup) {
		return (theGroup.getName() == "economy");
	});
	updateEconomyIdeas(*foundGroup, majorIdeologies);

	foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup) {
		return (theGroup.getName() == "trade_laws");
	});
	updateTradeLaws(*foundGroup, majorIdeologies);

	foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup) {
		return (theGroup.getName() == "country");
	});
	updateGeneralIdeas(*foundGroup, majorIdeologies);

	foundGroup = std::find_if(generalIdeas.begin(), generalIdeas.end(), [](auto& theGroup) {
		return (theGroup.getName() == "hidden_ideas");
	});
	updateHiddenIdeas(*foundGroup, majorIdeologies);

	if (auto foundIdeologicalGroup = ideologicalIdeas.find("neutrality");
		 foundIdeologicalGroup != ideologicalIdeas.end())
	{
		updateNeutralIdeas(foundIdeologicalGroup->second, majorIdeologies);
	}
}