/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <exception>
#include <fstream>

#include <elft_randimpl.h>

#ifdef DEBUG
#include <iostream>
#include <libelft_output.h>
#endif /* DEBUG */

ELFT::RandomImplementation::ConfigurationParameters
ELFT::RandomImplementation::Util::loadConfiguration(
    const std::filesystem::path &configurationDirectory)
{
	const auto path = configurationDirectory /
	    RandomImplementation::Constants::configFileName;
	if (!std::filesystem::exists(path))
		throw std::runtime_error{"Configuration file (" +
		    path.filename().string() + ") is not present in "
		    "configuration directory (" + path.parent_path().string() +
		    ")."};

	std::ifstream file{path};
	ConfigurationParameters params{};
	file >> params.seed;
	if (!file)
		throw std::runtime_error{"Couldn't read from configuration"};

	return (params);
}

std::vector<ELFT::RandomImplementation::Tmpl>
ELFT::RandomImplementation::Util::parseTemplate(
    const std::filesystem::path &pathToTemplate)
{
	std::ifstream file{pathToTemplate,
	    std::ifstream::ate | std::ifstream::binary};
	if (!file)
		return {};
	file.unsetf(std::ifstream::skipws);

	const auto size = file.tellg();
	if (size == -1)
		return {};

	std::vector<std::byte> templateData{};
	templateData.resize(static_cast<std::vector<std::byte>::size_type>(
	    size));
	file.seekg(std::ifstream::beg);
	file.read(reinterpret_cast<char *>(templateData.data()), size);

	return (parseTemplate(templateData));
}

std::vector<ELFT::RandomImplementation::Tmpl>
ELFT::RandomImplementation::Util::parseTemplate(
    const std::vector<std::byte> &templateData)
{
	std::vector<Tmpl> templates{};
	auto it = templateData.cbegin();

	/* First thing in template is string name. Read until null terminator */
	std::string candidateIdentifier{};
	while ((it != templateData.cend()) && (static_cast<char>(*it) != '\0'))
		candidateIdentifier += (static_cast<char>(*it++));
	++it;

	do {
		Tmpl t{};
		t.candidateIdentifier = candidateIdentifier;
		t.inputIdentifier = static_cast<uint8_t>(*it++);
		t.frgp = static_cast<FrictionRidgeGeneralizedPosition>(*it++);
		t.size = static_cast<uint8_t>(*it++);
		templates.push_back(t);
		std::advance(it, t.size);
	} while (it != templateData.cend());

	return (templates);
}

ELFT::ReturnStatus
ELFT::RandomImplementation::Util::writeTemplate(
    const std::filesystem::path &directory,
    const std::vector<std::byte> &templateData,
    const bool truncate)
{
	const auto identifier = Util::parseTemplate(templateData).front().
	    candidateIdentifier;

	std::ofstream::openmode mode = std::ofstream::out |
	    std::ofstream::binary;
	if (truncate)
		mode |= std::ofstream::trunc;
	std::ofstream file{directory / identifier, mode};
	if (!file)
		return {ReturnStatus::Result::Failure, "Unable to create "
		    "template identifier '" + identifier + "'"};

	if (!file.write(reinterpret_cast<const char *>(templateData.data()),
	    static_cast<std::streamsize>(templateData.size())))
		return {ReturnStatus::Result::Failure, "Unable to write to "
		    "identifier '" + identifier + "'"};

	return {};
}

#ifdef DEBUG
std::ostream&
ELFT::RandomImplementation::Util::operator<<(
    std::ostream &s,
    const Tmpl &t)
{
	return (s << "Candidate Identifier = " << t.candidateIdentifier <<
	    "\nInput Identifier = " <<
	    static_cast<uint16_t>(t.inputIdentifier) << '\n' << "FRGP = " <<
	    t.frgp << "\nSize = " << static_cast<uint16_t>(t.size));
}

#endif /* DEBUG */

/******************************************************************************/

ELFT::RandomImplementation::ExtractionImplementation::ExtractionImplementation(
    const std::filesystem::path &configurationDirectory) :
    ELFT::ExtractionInterface(),
    rng{RandomImplementation::Util::loadConfiguration(
        configurationDirectory).seed}
{

}

ELFT::ExtractionInterface::SubmissionIdentification
ELFT::RandomImplementation::ExtractionImplementation::getIdentification()
    const
{
	ExtractionInterface::SubmissionIdentification si{};

	/*
	 * Required.
	 */
	si.versionNumber = RandomImplementation::Constants::versionNumber;
	si.libraryIdentifier = RandomImplementation::Constants::
	    libraryIdentifier;

	/*
	 * Optional.
	 */
	ProductIdentifier exemplarID{};
	ProductIdentifier::CBEFFIdentifier exemplarCBEFF{};
	exemplarCBEFF.owner = RandomImplementation::Constants::productOwner;
	exemplarCBEFF.algorithm = 0xD1A7;
	exemplarID.cbeff = exemplarCBEFF;
	exemplarID.marketing = "RandomImplementation Exemplar Extractor 1.0";
	si.exemplarAlgorithmIdentifier = exemplarID;

	ProductIdentifier latentID{};
	ProductIdentifier::CBEFFIdentifier latentCBEFF{};
	latentCBEFF.owner = RandomImplementation::Constants::productOwner;
	latentCBEFF.algorithm  = 0xD1AC;
	latentID.cbeff = latentCBEFF;
	latentID.marketing = "RandomImplementation Latent Extractor 1.0";
	si.latentAlgorithmIdentifier = latentID;

	return (si);
}

ELFT::CreateTemplateResult
ELFT::RandomImplementation::ExtractionImplementation::createTemplate(
    const ELFT::TemplateType templateType,
    const std::string &identifier,
    const std::vector<std::tuple<
        std::optional<ELFT::Image>, std::optional<ELFT::EFS>>> &samples)
    const
{
	std::vector<std::byte> combinedTemplate{};
	for (const auto &c : identifier)
		combinedTemplate.push_back(static_cast<std::byte>(c));
	combinedTemplate.push_back(static_cast<std::byte>('\0'));

	for (const auto &sample : samples) {
		/* Record identifier */
		if (std::get<std::optional<Image>>(sample))
			combinedTemplate.push_back(static_cast<std::byte>(
			    std::get<std::optional<Image>>(sample)->
			    identifier));
		else if (std::get<std::optional<EFS>>(sample))
			combinedTemplate.push_back(static_cast<std::byte>(
			    std::get<std::optional<EFS>>(sample)->identifier));
		else
			return {{ReturnStatus::Result::Failure,
			    "Neither Image nor EFS data was provided."}, {}};

		/* Record samplePosition */
		if (std::get<std::optional<EFS>>(sample))
			combinedTemplate.push_back(static_cast<std::byte>(
			    std::underlying_type<
			    FrictionRidgeGeneralizedPosition>::type(
			    std::get<std::optional<EFS>>(sample)->frgp)));
		else
			combinedTemplate.push_back(static_cast<std::byte>(
			    std::underlying_type<
			    FrictionRidgeGeneralizedPosition>::type(
			    FrictionRidgeGeneralizedPosition::
			    UnknownFinger)));

		/* Generare a random amount of 0s and record */
		const uint8_t templateSize{static_cast<uint8_t>(
		    this->rng() % UINT8_MAX)};
		combinedTemplate.push_back(static_cast<std::byte>(
		    templateSize));
		combinedTemplate.insert(combinedTemplate.end(),
		    templateSize, {});
	}

	return {{}, combinedTemplate};
}

std::optional<std::tuple<ELFT::ReturnStatus, std::vector<ELFT::TemplateData>>>
ELFT::RandomImplementation::ExtractionImplementation::extractTemplateData(
    const ELFT::TemplateType templateType,
    const ELFT::CreateTemplateResult &templateResult)
    const
{
	const auto templates = Util::parseTemplate(templateResult.data);

	std::vector<TemplateData> tds{};
	for (const auto &t : templates) {
		TemplateData td{};
		td.candidateIdentifier = t.candidateIdentifier;
		td.inputIdentifier = t.inputIdentifier;

		/* Make up a couple features */
		EFS efs{};
		if (templateType == TemplateType::Probe) {
			efs.orientation = (this->rng() % 180);
			if (*efs.orientation % 2)
				(*efs.orientation) = static_cast<int16_t>(
				    efs.orientation.value() * -1);
		}

		const uint8_t numMinutiae{static_cast<uint8_t>(
		    this->rng() % UINT8_MAX)};
		if (numMinutiae > 0) {
			efs.minutiae = std::vector<Minutia>{};
			efs.minutiae->reserve(numMinutiae);
			for (uint8_t i{}; i < numMinutiae; ++i) {
				Minutia m{};
				m.coordinate.x = static_cast<uint16_t>(
				    this->rng() % 1000);
				m.coordinate.y = static_cast<uint16_t>(
				    this->rng() % 1000);
				m.theta = static_cast<uint16_t>(
				    this->rng() % 360);

				efs.minutiae->push_back(m);
			}
		}

		td.efs = efs;
		tds.push_back(td);
	}

	return (std::make_tuple(ReturnStatus{}, tds));
}

ELFT::CreateTemplateResult
ELFT::RandomImplementation::ExtractionImplementation::mergeTemplates(
    const std::vector<std::vector<std::byte>> &templates)
    const
{
	/*
	 * Our templates work by having the identifier first, followed
	 * by N pieces of data. Start by appending all templates without
	 * the subject identifier.
	 */
	std::vector<std::byte> combinedTemplate{};
	for (const auto &tmpl : templates) {
		const std::string id{Util::parseTemplate(tmpl).front().
		    candidateIdentifier};

		auto tmplCopy = tmpl;
		tmplCopy.erase(tmplCopy.begin(),
		    std::next(tmplCopy.begin(),
		    static_cast<int>(id.length()) + 1));
		combinedTemplate.insert(combinedTemplate.end(),
		    tmplCopy.begin(), tmplCopy.end());
	}

	/* Then add the subject identifier to the beginning. */
	const std::string id{Util::parseTemplate(templates.front()).front().
	    candidateIdentifier};
	std::vector<std::byte> idBytes{};
	for (const auto &c : id)
		idBytes.push_back(static_cast<std::byte>(c));
	idBytes.push_back(static_cast<std::byte>('\0'));
	combinedTemplate.insert(combinedTemplate.begin(), idBytes.begin(),
	    idBytes.end());

#ifdef DEBUG
	/* Output the merged */
	using ELFT::RandomImplementation::Util::operator<<;
	std::cout << "Started with " << templates.size() << " templates:\n";
	for (const auto &t : templates)
		std::cout << Util::parseTemplate(t).front() << '\n';

	std::cout << "\nMerged to 1 template:\n";
	const auto parsedCombined = Util::parseTemplate(combinedTemplate);
	for (const auto &t : parsedCombined)
		std::cout << t << '\n';
#endif /* DEBUG */

	return {{}, combinedTemplate};
}

ELFT::ReturnStatus
ELFT::RandomImplementation::ExtractionImplementation::createReferenceDatabase(
    const std::vector<std::vector<std::byte>> &referenceTemplates,
    const std::filesystem::path &databaseDirectory,
    const uint64_t maxSize)
    const
{
	/* First, do a rough check that we have enough space */
	static const uint8_t maxFingerSize{UINT8_MAX};
	static const uint8_t estimatedNumImagesPerSubject{20};
	if ((maxFingerSize * estimatedNumImagesPerSubject *
	    referenceTemplates.size()) > maxSize)
		return {ReturnStatus::Result::Failure, "Given a maximum "
		    "template size of " + std::to_string(maxFingerSize) + "b "
		    "and an estimated number of fingers per subject of " +
		    std::to_string(estimatedNumImagesPerSubject) + ", " +
		    std::to_string(maxSize) + "b does not seem to be enough."};

	for (const auto &combinedTemplate : referenceTemplates) {
		const auto rs = Util::writeTemplate(databaseDirectory,
		    combinedTemplate);
		if (!rs)
			return (rs);
	}

	return {};
}

std::shared_ptr<ELFT::ExtractionInterface>
ELFT::ExtractionInterface::getImplementation(
    const std::filesystem::path &configurationDirectory)
{
	return (std::make_shared<
	    RandomImplementation::ExtractionImplementation>(
	    configurationDirectory));
}

/******************************************************************************/

ELFT::RandomImplementation::SearchImplementation::SearchImplementation(
    const std::filesystem::path &configurationDirectory,
    const std::filesystem::path &databaseDirectory) :
    ELFT::SearchInterface(),
    databaseDirectory{databaseDirectory},
    rng{RandomImplementation::Util::loadConfiguration(
        configurationDirectory).seed}
{

}

std::optional<ELFT::ProductIdentifier>
ELFT::RandomImplementation::SearchImplementation::getIdentification()
    const
{
	ProductIdentifier id{};
	id.marketing = "RandomImplementation Matcher 1.0";
	ProductIdentifier::CBEFFIdentifier cbeff{};
	cbeff.owner = RandomImplementation::Constants::productOwner;
	cbeff.algorithm = 0x0101;
	id.cbeff = cbeff;

	return (id);
}

std::tuple<ELFT::ReturnStatus, bool>
ELFT::RandomImplementation::SearchImplementation::exists(
    const std::string &identifier)
    const
{
	return {{},
	    std::filesystem::exists(this->databaseDirectory / identifier)};
}

ELFT::ReturnStatus
ELFT::RandomImplementation::SearchImplementation::insert(
    const std::string &identifier,
    const std::vector<std::byte> &referenceTemplate)
{
	auto [rs, truncate] = this->exists(identifier);
	if (!rs)
		return {ReturnStatus::Result::Failure, "Could not determine "
		    "if '" + identifier + "' exists in database (message "
		    "was: " + (rs.message ? *rs.message : "")};

	return (Util::writeTemplate(databaseDirectory, referenceTemplate,
	    truncate));
}

ELFT::ReturnStatus
ELFT::RandomImplementation::SearchImplementation::remove(
    const std::string &identifier)
{
	auto [rs, exists] = this->exists(identifier);
	if (!rs)
		return {ReturnStatus::Result::Failure, "Could not determine "
		    "if '" + identifier + "' exists in database (message "
		    "was: " + (rs.message ? *rs.message : "") + ')'};

	if (!exists)
		return {};

	std::error_code error{};
	std::filesystem::remove(this->databaseDirectory / identifier, error);
	if (error)
		return {ReturnStatus::Result::Failure, "Could not remove '" +
		    identifier + "' from database (error was: "+
		    error.message() + ')'};
	return {};
}

ELFT::SearchResult
ELFT::RandomImplementation::SearchImplementation::search(
    const std::vector<std::byte> &probeTemplate,
    const uint16_t maxCandidates)
    const
{
	ELFT::SearchResult result{};
	result.candidateList.reserve(maxCandidates);

	/* Get some real candidate names */
	for (const auto &f : std::filesystem::directory_iterator(
	    this->databaseDirectory)) {
		const auto &templates = Util::parseTemplate(f.path());
		const auto &matchingTemplate = templates.at(
		    this->rng() % templates.size());

		/* Set a realistic FRGP for slap templates */
		auto frgp = matchingTemplate.frgp;
		switch (frgp) {
		case FrictionRidgeGeneralizedPosition::RightFour:
			frgp = static_cast<FrictionRidgeGeneralizedPosition>(
			    (this->rng() % 4) + 2);
			break;
		case FrictionRidgeGeneralizedPosition::LeftFour:
			frgp = static_cast<FrictionRidgeGeneralizedPosition>(
			    (this->rng() % 4) + 7);
			break;
		case FrictionRidgeGeneralizedPosition::RightAndLeftThumbs:
			frgp = static_cast<FrictionRidgeGeneralizedPosition>(
			    (this->rng() % 2) + 5);
			break;
		default:
			break;
		}

		result.candidateList.push_back({
		    matchingTemplate.candidateIdentifier, frgp,
		    static_cast<double>(this->rng() % UINT16_MAX)});

		if (result.candidateList.size() == maxCandidates)
			break;
	}

	result.decision = ((this->rng() % 2) == 0);

	return (result);
}

std::optional<std::tuple<ELFT::ReturnStatus,
    std::vector<std::vector<ELFT::Correspondence>>>>
ELFT::RandomImplementation::SearchImplementation::extractCorrespondence(
    const std::vector<std::byte> &probeTemplate,
    const SearchResult &searchResult)
    const
{
	const auto probe = Util::parseTemplate(probeTemplate).front();
	std::vector<std::vector<ELFT::Correspondence>> allCorrespondence{};
	allCorrespondence.reserve(searchResult.candidateList.size());

	for (const auto &c : searchResult.candidateList) {
		const auto &referenceTemplates = Util::parseTemplate(
		    this->databaseDirectory / c.identifier);

		/* NOTE: See NOTE below. This won't line up. */
		bool onlySlaps{true};
		for (const auto &tmpl : referenceTemplates) {
			if ((static_cast<uint8_t>(tmpl.frgp) < 13) ||
			    (static_cast<uint8_t>(tmpl.frgp) > 15)) {
				onlySlaps = false;
				break;
			}
			/*
			 * NOTE: See NOTE below. If we did something like this
			 *       in production, we'd need to include an ROI.
			 */
		}

		for (const auto &tmpl : referenceTemplates) {
			/* Find the correct subtemplate within the reference */
			if (onlySlaps) {
				using frgp_t = std::underlying_type<
				    FrictionRidgeGeneralizedPosition>::type;
				const auto c_frgp = static_cast<frgp_t>(c.frgp);
				const auto tmpl_frgp = static_cast<frgp_t>(
				    tmpl.frgp);

				if (c_frgp >= 2 && c_frgp <= 5) {
					if (tmpl_frgp != 13)
						continue;
				} else if (c_frgp >= 7 && c_frgp <= 10) {
					if (tmpl_frgp != 14)
						continue;
				} else {
					if (tmpl_frgp != 15)
						continue;
				}
			} else {
				if (tmpl.frgp != c.frgp)
					continue;
			}

			const uint8_t numMinutiae{static_cast<uint8_t>(
			    this->rng() % UINT8_MAX)};

			/*
			 * NOTE: ELFT requires that corresponding minutiae be
			 *       from the set of minutiae returned in
			 *       extractTemplateData() for both templates.
			 *       There's no easy way to do so in this example.
			 */
			std::vector<Correspondence> candidateCorr{};
			candidateCorr.reserve(numMinutiae);
			for (uint8_t i{}; i < numMinutiae; ++i) {
				Correspondence singleCorr{};
				singleCorr.probeInputIdentifier =
				    probe.inputIdentifier;
				singleCorr.referenceInputIdentifier =
				    tmpl.inputIdentifier;

				singleCorr.probeMinutia.coordinate.x =
				    static_cast<uint16_t>(this->rng() % 1000);
				singleCorr.probeMinutia.coordinate.y =
				    static_cast<uint16_t>(this->rng() % 1000);
				singleCorr.probeMinutia.theta =
				    static_cast<uint16_t>(this->rng() % 360);

				singleCorr.referenceMinutia.coordinate.x =
				    static_cast<uint16_t>(this->rng() % 1000);
				singleCorr.referenceMinutia.coordinate.y =
				    static_cast<uint16_t>(this->rng() % 1000);
				singleCorr.referenceMinutia.theta =
				    static_cast<uint16_t>(this->rng() % 16);

				candidateCorr.push_back(singleCorr);
			}
			allCorrespondence.push_back(candidateCorr);
			break;
		}
	}

	return (std::make_tuple(ReturnStatus{}, allCorrespondence));
}

std::shared_ptr<ELFT::SearchInterface>
ELFT::SearchInterface::getImplementation(
    const std::filesystem::path &configurationDirectory,
    const std::filesystem::path &databaseDirectory)
{
	return (std::make_shared<RandomImplementation::SearchImplementation>(
	    configurationDirectory, databaseDirectory));
}
