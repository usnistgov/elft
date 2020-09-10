/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef ELFT_RANDIMPL_H_
#define ELFT_RANDIMPL_H_

#include <random>

#include <elft.h>

#ifdef DEBUG
#include <ostream>
#endif /* DEBUG */

namespace ELFT
{
	namespace RandomImplementation
	{
		/** Information contained in configuration file. */
		struct ConfigurationParameters
		{
			/** Random-number engine seed. */
			std::uint_fast32_t seed{};
		};

		/** Template format */
		struct Tmpl
		{
			/** Candidate name. */
			std::string candidateIdentifier{};
			/** Input identifier from createTemplate(). */
			uint8_t inputIdentifier{};
			/** Finger position */
			FrictionRidgeGeneralizedPosition frgp{};
			/**
			 * After this byte, remaining number of bytes for this
			 * template.
			 */
			uint8_t size{};
		};

		namespace Constants
		{
			uint16_t versionNumber{0x0001};
			uint16_t productOwner{0x000F};
			std::string libraryIdentifier{"randimpl"};
			std::string configFileName{"seed"};
		}

		namespace Util
		{
			/**
			 * @brief
			 * Read and parse the configuration file.
			 *
			 * @param configurationDirectory
			 * `configurationDirectory` from constructor.
			 */
			ConfigurationParameters
			loadConfiguration(
			    const std::filesystem::path
			        &configurationDirectory);

			/**
			 * @brief
			 * Extract individual "native" templates from single
			 * "ELFT" template on disk.
			 *
			 * @param pathToTemplate
			 * Filesystem location of template on disk.
			 *
			 * @return
			 * Collection of individual "native" templates.
			 */
			std::vector<Tmpl>
			parseTemplate(
			    const std::filesystem::path &pathToTemplate);

			/**
			 * @brief
			 * Extract individual "native" templates from single
			 * "ELFT" template in memory.
			 *
			 * @param templateData
			 * Combined "ELFT" template created in createTemplate().
			 *
			 * @return
			 * Collection of individual "native" templates.
			 */
			std::vector<Tmpl>
			parseTemplate(
			    const std::vector<std::byte> &templateData);

			/**
			 * @brief
			 * Write template data to disk.
			 *
			 * @param directory
			 * Directory at which to write `templateData`. The name
			 * of the file in the directory will be parsed from
			 * `templateData`.
			 * @param templateData
			 * Combined "ELFT" template created in createTemplate().
			 * @param truncate
			 * Whether or not to truncate file if it already exists.
			 *
			 * @return
			 * Status of completing this operation.
			 */
			ReturnStatus
			writeTemplate(
			    const std::filesystem::path &directory,
			    const std::vector<std::byte> &templateData,
			    const bool truncate = true);

#ifdef DEBUG
			/**
			 * @brief
			 * Print textual description of Tmpl to stream.
			 *
			 * @param s
			 * Stream to append to.
			 * @param t
			 * Tmpl to describe.
			 *
			 * @return
			 * `s` with `t` appended.
			 */
			std::ostream&
			operator<<(
			    std::ostream &s,
			    const Tmpl &t);
#endif /* DEBUG */
		}

		class ExtractionImplementation : public ExtractionInterface
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
				std::optional<Image>, std::optional<EFS>>>
				&fingers)
			    const
			    override;

			std::optional<std::vector<TemplateData>>
			extractTemplateData(
			    const TemplateType templateType,
			    const CreateTemplateResult &templateResult)
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

			ExtractionImplementation(
			    const std::filesystem::path
			        &configurationDirectory);

		private:
			mutable std::mt19937_64 rng{};
		};

		class SearchImplementation : public SearchInterface
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
			    const std::string &identifier,
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

			std::optional<std::vector<std::vector<Correspondence>>>
			extractCorrespondence(
			    const std::vector<std::byte> &probeTemplate,
			    const SearchResult &searchResult)
			    const
			    override;

			SearchImplementation(
			    const std::filesystem::path
			        &configurationDirectory,
			    const std::filesystem::path &databaseDirectory);

		private:
			const std::filesystem::path databaseDirectory{};
			mutable std::mt19937_64 rng{};
		};
	}
}

#endif /* ELFT_RANDIMPL_H_ */
