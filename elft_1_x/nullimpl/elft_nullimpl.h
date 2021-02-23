/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef ELFT_NULLIMPL_H_
#define ELFT_NULLIMPL_H_

#include <elft.h>

namespace ELFT
{
	namespace NullImplementationConstants
	{
		uint16_t versionNumber{0x0001};
		uint16_t productOwner{0x000F};
		std::string libraryIdentifier{"nullimpl"};
	}

	class NullExtractionImplementation : public ExtractionInterface
	{
	public:
		SubmissionIdentification
		getIdentification()
		    const
		    override;

		CreateTemplateResult
		createTemplate(
		    const TemplateType templateType,
		    const std::string &identifier,
		    const std::vector<std::tuple<
       		        std::optional<Image>, std::optional<EFS>>> &samples)
		    const
		    override;

		std::optional<std::tuple<ReturnStatus,
		    std::vector<TemplateData>>>
		extractTemplateData(
		    const TemplateType templateType,
		    const CreateTemplateResult &templateResult)
		    const
		    override;

		CreateTemplateResult
		mergeTemplates(
		    const std::vector<std::vector<std::byte>> &templates)
		    const
		    override;

		ReturnStatus
		createReferenceDatabase(
		    const std::vector<std::vector<std::byte>>
		        &referenceTemplates,
		    const std::filesystem::path &databaseDirectory,
		    const uint64_t maxSize)
		    const
		    override;

		NullExtractionImplementation(
		    const std::filesystem::path &configurationDirectory = {});

	private:
		const std::filesystem::path configurationDirectory{};
	};

	class NullSearchImplementation : public SearchInterface
	{
	public:
		std::optional<ProductIdentifier>
		getIdentification()
		    const
		    override;

		std::tuple<ReturnStatus, bool>
		exists(
		    const std::string &identifier)
		    const
		    override;

		ReturnStatus
		insert(
		    const std::vector<std::byte> &referenceTemplate)
		    override;

		ReturnStatus
		remove(
		    const std::string &identifier)
		    override;

		SearchResult
		search(
		    const std::vector<std::byte> &probeTemplate,
		    const uint16_t maxCandidates)
		    const
		    override;

		std::optional<std::tuple<ReturnStatus,
		    std::vector<std::vector<Correspondence>>>>
		extractCorrespondence(
		    const std::vector<std::byte> &probeTemplate,
		    const SearchResult &searchResult)
		    const
		    override;

		NullSearchImplementation(
		    const std::filesystem::path &configurationDirectory,
		    const std::filesystem::path &databaseDirectory);

	private:
		const std::filesystem::path configurationDirectory{};
		const std::filesystem::path databaseDirectory{};
	};
}

#endif /* ELFT_NULLIMPL_H_ */
