/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <algorithm>
#include <exception>
#include <fstream>
#include <future>
#include <unordered_map>

#include <elft_randimpl.h>

#ifdef DEBUG
#include <iostream>
#include <libelft_output.h>
#endif /* DEBUG */


std::filesystem::path
ELFT::RandomImplementation::Util::getDirectoryForIdentifier(
    const std::string &identifier)
{
	/*
	 * We will get bad file system performance if we store millions of
	 * files in a single directory. To balance this out in this example, we
	 * will make subdirectories based on the first characters of the
	 * identifier. If you don't make a database file, you might do something
	 * similar based on an MD5 checksum of the identifier string.
	 */
	static const uint8_t charactersToConsider{8};

	if (identifier.length() <= charactersToConsider)
		return (identifier);

	return (identifier.substr(0, 2) + '/' + identifier.substr(2, 2) + '/' +
	    identifier.substr(4, 2) + '/' + identifier.substr(6, 2));
}

std::filesystem::path
ELFT::RandomImplementation::Util::getDirectoryForTemplate(
    const std::vector<std::byte> &templateData)
{
	return (getDirectoryForIdentifier(Util::parseTemplate(templateData).
	    front().candidateIdentifier));
}

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

std::tuple<ELFT::ReturnStatus, std::vector<std::byte>>
ELFT::RandomImplementation::Util::readTemplate(
    const std::filesystem::path &directory,
    const std::string &identifier)
{
	std::ifstream file{directory / identifier,
	    std::ifstream::ate | std::ifstream::binary};
	if (!file)
		return {{ReturnStatus::Result::Failure, "Unable to open "
		    "template identifier '" + identifier + "'"}, {}};

	file.unsetf(std::ifstream::skipws);

	const auto size = file.tellg();
	if (size == -1)
		return {{ReturnStatus::Result::Failure, "Unable to open "
		    "template identifier '" + identifier + "'"}, {}};

	std::vector<std::byte> buf{};
	buf.resize(static_cast<std::vector<std::byte>::size_type>(size));
	file.seekg(std::ifstream::beg);
	file.read(reinterpret_cast<char*>(buf.data()), size);

	return {{}, buf};
}

ELFT::ReturnStatus
ELFT::RandomImplementation::Util::writeTemplate(
    const std::filesystem::path &directory,
    const std::vector<std::byte> &templateData,
    const bool truncate)
{
	const auto identifier = Util::parseTemplate(templateData).front().
	    candidateIdentifier;

	if (!std::filesystem::is_directory(directory)) {
		if (std::filesystem::exists(directory)) {
			return {ReturnStatus::Result::Failure,
			    "Unexpected file at " + directory.string()};
		}

		if (!std::filesystem::create_directories(directory)) {
			return {ReturnStatus::Result::Failure,
			    "Could not create directory " +
			    directory.string()};
		}
	}

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

	CreateTemplateResult result = {{}, combinedTemplate};

	/*
	 * We can set TemplateData here, or wait to have extractTemplateData
	 * called later.
	 */
	const auto templateData = this->extractTemplateData(templateType,
	    result);
	if (templateData && std::get<ReturnStatus>(*templateData))
		result.extractedData = std::get<std::vector<
		    TemplateData>>(*templateData);

	return (result);
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

ELFT::ReturnStatus
ELFT::RandomImplementation::ExtractionImplementation::createReferenceDatabase(
    const TemplateArchive &referenceTemplates,
    const std::filesystem::path &databaseDirectory,
    const uint64_t maxSize)
    const
{
	/* First, do a rough check that we have enough space */
	std::ifstream archive{referenceTemplates.archive,
	    std::ios_base::in | std::ios_base::ate | std::ios_base::binary};
	if (!archive.is_open())
		return {ReturnStatus::Result::Failure, "Could not open "
		    "TemplateArchive.archive: " +
		    referenceTemplates.archive.string()};
	const auto templateBytes = archive.tellg();
	archive.close();

	if (templateBytes == std::ifstream::pos_type(-1))
		return {ReturnStatus::Result::Failure, "Size of " +
		    referenceTemplates.archive.string() + " could not be "
		    "determined"};
	if (maxSize < static_cast<uint64_t>(
	    static_cast<double>(templateBytes) * 1.1))
		return {ReturnStatus::Result::Failure, "Given " +
		    std::to_string(templateBytes) + " bytes of templates, " +
		    std::to_string(maxSize) + " is not enough storage space "
		    "for the reference database. Estimated size required is "
		    "1.1x the size of templates."};

	struct ManifestEntryPosition
	{
		std::streamsize length{};
		std::ifstream::pos_type offset{};
	};
	std::unordered_map<std::string, ManifestEntryPosition> templates{};

	/*
	 * Read manifest into a map. Note that this may contain many millions of
	 * entries.
	 */
	std::ifstream manifest{referenceTemplates.manifest};
	std::string id{}, length{}, offset{};
	while (true) {
		manifest >> id >> length >> offset;
		if (!manifest)
			break;

		templates[id] = ManifestEntryPosition{
		    static_cast<std::streamsize>(std::stoll(length)),
		    static_cast<std::ifstream::pos_type>(std::stoll(offset))};
	}

	/*
	 * NOTE: There will be millions of identifiers. Avoid putting everything
	 * in a single directory. Preferably, use some sort of database file.
	 * One such database file, TemplateArchive, is provided to you! Use it!
	 *
	 * NOTE: This method should take advantage of available hardware.
	 * A single thread writing to disk will likely not complete in the
	 * required amount of time.
	 */

	auto threadWrite = [](
	    const std::filesystem::path &archivePath,
	    const std::string &dbDir,
	    const std::unordered_map<std::string, ManifestEntryPosition>::
	        const_iterator &begin,
	    const std::unordered_map<std::string, ManifestEntryPosition>::
	        const_iterator &end) ->
	    ReturnStatus{
		try {
			std::ifstream archive{archivePath,
			    std::ios_base::in | std::ios_base::binary};

			std::for_each(begin, end, [&dbDir, &archive](
			    const auto &manifestEntry) {
				/* Read the template from the archive */
				std::vector<std::byte> combinedTemplate{};
				combinedTemplate.resize(static_cast<
				    std::vector<std::byte>::size_type>(
				    manifestEntry.second.length));
				archive.seekg(manifestEntry.second.offset);
				archive.read(reinterpret_cast<char*>(
				    combinedTemplate.data()),
				    manifestEntry.second.length);

			    	/* Write it back out as an individual file */
				const auto rs = Util::writeTemplate(
				    dbDir / Util::getDirectoryForTemplate(
				    combinedTemplate), combinedTemplate);
				if (!rs)
					throw rs;
			});
		} catch (const ReturnStatus &rs) {
			return (rs);
		}

		return {};
	};

	/*
	 * For small databases (e.g., validation), you might not need to
	 * multithread.
	 */
	bool shouldMultithread{true};
	if (templateBytes < 10000)
		shouldMultithread = false;
	if (!shouldMultithread)
		return (threadWrite(referenceTemplates.archive,
		    databaseDirectory, templates.cbegin(), templates.cend()));

	const auto numCores = std::thread::hardware_concurrency() - 1;
	std::vector<std::future<ReturnStatus>> futures{};
	futures.reserve(numCores);

	const uint64_t tmplPerThread{static_cast<uint64_t>(std::floor(
	    templates.size() / numCores))};
	for (unsigned int i{0}; i < numCores; ++i) {
		if ((((i + 1) * tmplPerThread)) > PTRDIFF_MAX)
			return {ReturnStatus::Result::Failure,
			    "Iterator size too large for ptrdiff_t"};

		futures.emplace_back(std::async(std::launch::async,
		    threadWrite, referenceTemplates.archive, databaseDirectory,
		    std::next(templates.cbegin(),
		        static_cast<std::ptrdiff_t>(i * tmplPerThread)),
		    ((i == numCores - 1) ?
		        templates.cend() :
		        std::next(templates.cbegin(),
		        static_cast<std::ptrdiff_t>(
		            ((i + 1) * tmplPerThread))))));
	}

	ReturnStatus rs{};
	for (unsigned int i{0}; i < futures.size(); ++i) {
		const auto futureRS = futures.at(i).get();

		if (futureRS.message) {
			if (!rs.message)
				rs.message = std::string{};

			*rs.message = "Thread " + std::to_string(i) + ": " +
			    *futureRS.message + ' ';
		}

		if (!futureRS)
			rs.result = ReturnStatus::Result::Failure;
	}
	if (rs.message && !rs.message->empty())
		rs.message->pop_back();

	return (rs);
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

ELFT::ReturnStatus
ELFT::RandomImplementation::SearchImplementation::load(
    const uint64_t maxSize)
{
	/*
	 * XXX: This method is not implemented, because this trivial algorithm
	 *      only reads from disk. You shouldn't be this simple, so we
	 *      suggest implementing something like the below.
	 */

	/* Can we load *everything* into RAM? */
	const uint64_t templateBytes = /* how much space do you need */ 1;

	if (maxSize < templateBytes) {
		/* We could load the entire database into RAM. */
	} else {
		/*
		 * We could load some of the database, but the rest would
		 * required reading from disk.
		 */
	}

	return {};
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

ELFT::SearchResult
ELFT::RandomImplementation::SearchImplementation::search(
    const std::vector<std::byte> &probeTemplate,
    const uint16_t maxCandidates)
    const
{
	ELFT::SearchResult result{};
	result.candidateList.reserve(maxCandidates);

	/* Get some real candidate names */
	for (const auto &f : std::filesystem::recursive_directory_iterator(
	    this->databaseDirectory)) {
		if (!f.is_regular_file())
			continue;

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

	/*
	 * We can set correspondence here or wait to have extractCorrespondence
	 * called later.
	 */
	const auto correspondence = this->extractCorrespondence(
	    probeTemplate, result);
	if (correspondence && correspondence->status)
		result.correspondence = correspondence->data;

	return (result);
}

std::optional<ELFT::CorrespondenceResult>
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
		    this->databaseDirectory / Util::getDirectoryForIdentifier(
		    c.identifier) / c.identifier);

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

			std::vector<Correspondence> candidateCorr{};
			candidateCorr.reserve(numMinutiae);
			for (uint8_t i{}; i < numMinutiae; ++i) {
				Correspondence singleCorr{};

				singleCorr.type = CorrespondenceType::Definite;

				singleCorr.probeIdentifier =
				    probe.candidateIdentifier;
				singleCorr.referenceIdentifier =
				    tmpl.candidateIdentifier;

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

	return (CorrespondenceResult{ReturnStatus{},
	    {allCorrespondence, false}});

}

std::shared_ptr<ELFT::SearchInterface>
ELFT::SearchInterface::getImplementation(
    const std::filesystem::path &configurationDirectory,
    const std::filesystem::path &databaseDirectory)
{
	return (std::make_shared<RandomImplementation::SearchImplementation>(
	    configurationDirectory, databaseDirectory));
}
