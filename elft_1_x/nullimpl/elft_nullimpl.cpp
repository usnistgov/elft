/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <elft_nullimpl.h>

ELFT::NullExtractionImplementation::NullExtractionImplementation(
    const std::filesystem::path &configurationDirectory) :
    ELFT::ExtractionInterface(),
    configurationDirectory{configurationDirectory}
{

}

ELFT::ExtractionInterface::SubmissionIdentification
ELFT::NullExtractionImplementation::getIdentification()
    const
{
	ExtractionInterface::SubmissionIdentification si{};

	/*
	 * Required.
	 */
	si.versionNumber = NullImplementationConstants::versionNumber;
	si.libraryIdentifier = NullImplementationConstants::libraryIdentifier;

	/*
	 * Optional.
	 */
	ProductIdentifier exemplarID{};
	ProductIdentifier::CBEFFIdentifier exemplarCBEFF{};
	exemplarCBEFF.owner = NullImplementationConstants::productOwner;
	exemplarCBEFF.algorithm = 0x0F1A7;
	exemplarID.cbeff = exemplarCBEFF;
	exemplarID.marketing = "NullImplementation Exemplar Extractor 1.0";
	si.exemplarAlgorithmIdentifier = exemplarID;

	ProductIdentifier latentID{};
	ProductIdentifier::CBEFFIdentifier latentCBEFF{};
	latentCBEFF.owner = NullImplementationConstants::productOwner;
	latentCBEFF.algorithm  = 0x01AC;
	latentID.cbeff = latentCBEFF;
	latentID.marketing = "NullImplementation Latent Extractor 1.0";
	si.latentAlgorithmIdentifier = latentID;

	return (si);
}

ELFT::CreateTemplateResult
ELFT::NullExtractionImplementation::createTemplate(
    const ELFT::TemplateType templateType,
    const std::string &identifier,
    const std::vector<std::tuple<
        std::optional<ELFT::Image>, std::optional<ELFT::EFS>>> &samples)
    const
{
	return {};
}

std::optional<std::tuple<ELFT::ReturnStatus, std::vector<ELFT::TemplateData>>>
ELFT::NullExtractionImplementation::extractTemplateData(
    const ELFT::TemplateType templateType,
    const ELFT::CreateTemplateResult &templateResult)
    const
{
	return {};
}

ELFT::ReturnStatus
ELFT::NullExtractionImplementation::createReferenceDatabase(
    const TemplateArchive &referenceTemplates,
    const std::filesystem::path &databaseDirectory,
    const uint64_t maxSize)
    const
{
	try {
		std::filesystem::copy_file(referenceTemplates.archive,
		    databaseDirectory / "archive");
	} catch (const std::filesystem::filesystem_error &e) {
		return {ReturnStatus::Result::Failure, "Error when copying " +
		    referenceTemplates.archive.string() + ": " + e.what()};
	}

	try {
		std::filesystem::copy_file(referenceTemplates.manifest,
		    databaseDirectory / "manifest");
	} catch (const std::filesystem::filesystem_error &e) {
		return {ReturnStatus::Result::Failure, "Error when copying " +
		    referenceTemplates.manifest.string() + ": " + e.what()};
	}

	return {};
}

std::shared_ptr<ELFT::ExtractionInterface>
ELFT::ExtractionInterface::getImplementation(
    const std::filesystem::path &configurationDirectory)
{
	return (std::make_shared<NullExtractionImplementation>(
	    configurationDirectory));
}

/******************************************************************************/

ELFT::NullSearchImplementation::NullSearchImplementation(
    const std::filesystem::path &configurationDirectory,
    const std::filesystem::path &databaseDirectory) :
    ELFT::SearchInterface(),
    configurationDirectory{configurationDirectory},
    databaseDirectory{databaseDirectory}
{
	/* Do NOT load templates into RAM here */
}

ELFT::ReturnStatus
ELFT::NullSearchImplementation::load(
    const uint64_t maxSize)
{
	if (!std::filesystem::is_directory(this->databaseDirectory))
		throw std::runtime_error{this->databaseDirectory.string() + " "
		    "does not exist"};
	if (!std::filesystem::exists(this->databaseDirectory / "archive"))
		throw std::runtime_error{(this->databaseDirectory /
		    "archive").string() + " does not exist"};
	if (!std::filesystem::exists(this->databaseDirectory / "manifest"))
		throw std::runtime_error{(this->databaseDirectory /
		    "manifest").string() + " does not exist"};

	/* XXX: Load maxSize bytes of data into RAM. */

	return {};
}

std::optional<ELFT::ProductIdentifier>
ELFT::NullSearchImplementation::getIdentification()
    const
{
	/*
	 * Optional. `return {};` will suffice.
	 */

	ProductIdentifier id{};
	id.marketing = "NullImplementation Matcher 1.0";
	ProductIdentifier::CBEFFIdentifier cbeff{};
	cbeff.owner = NullImplementationConstants::productOwner;
	cbeff.algorithm = 0x0101;
	id.cbeff = cbeff;

	return (id);
}

ELFT::SearchResult
ELFT::NullSearchImplementation::search(
    const std::vector<std::byte> &probeTemplate,
    const uint16_t maxCandidates)
    const
{
	ELFT::SearchResult result{};
	result.candidateList.reserve(maxCandidates);
	result.decision = false;

	return (result);
}

std::optional<ELFT::CorrespondenceResult>
ELFT::NullSearchImplementation::extractCorrespondence(
    const std::vector<std::byte> &probeTemplate,
    const SearchResult &searchResult)
    const
{
	return {};
}

std::shared_ptr<ELFT::SearchInterface>
ELFT::SearchInterface::getImplementation(
    const std::filesystem::path &configurationDirectory,
    const std::filesystem::path &databaseDirectory)
{
	return (std::make_shared<NullSearchImplementation>(
	    configurationDirectory, databaseDirectory));
}
