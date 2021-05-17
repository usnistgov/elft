/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef ELFT_VALIDATION_H_
#define ELFT_VALIDATION_H_

#include <cstddef>
#include <filesystem>
#include <random>
#include <optional>
#include <string>
#include <vector>

#include <elft.h>
#include <elft_validation_data.h>

namespace ELFT::Validation
{
	/** Operations that this executable can perform. */
	enum class Operation
	{
		/** Extract features and make a fingerprint template. */
		Extract,
		/** Create reference database from templates. */
		CreateReferenceDatabase,
		/** Search the reference database. */
		Search,
		/** Print identification provided by ExtractionInterface. */
		Identify,
		/** Print identification provided by SearchInterface. */
		IdentifySearch,
		/** Print usage. */
		Usage
	};

	/** Arguments passed on the command line */
	struct Arguments
	{
		/** Number used to seed the random number generator. */
		std::mt19937_64::result_type randomSeed{std::random_device()()};
		/** Operation to be performed. */
		std::optional<Operation> operation{};
		/** Number of processes to run. */
		uint8_t numProcs{1};
		/** Configuration directory. */
		std::filesystem::path configDir{};
		/** Enrollment database directory. */
		std::filesystem::path dbDir{};
		/** Type of template to extract (Operation::Extract only) */
		std::optional<TemplateType> templateType{};
		/** Maximum number of {candidates, DB size}. */
		uint64_t maximum{};
		/** Directory where output will be written. */
		std::filesystem::path outputDir{"output"};
		/** Directory containing images from ELFT::Validation::Data. */
		std::filesystem::path imageDir{"images"};
	};

	/**
	 * @brief
	 * Call the appropriate starting method based on the operation argument
	 * passed on the command-line.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Exit status from the operation.
	 */
	int
	dispatchOperation(
	    const Arguments &args);

	/**
	 * @brief
	 * Return an element from the appropriate Data::Validation vector.
	 *
	 * @param imageIndex
	 * Element index in the ImageSet vector.
	 * @param templateType
	 * The type of template we want, which will let us switch to the
	 * appropriate ImageSet vector.
	 *
	 * @return
	 * Reference to the entry `imageIndex` from the appropriate Data::
	 * Validation vector based on `templateType`.
	 */
	const Data::ImageSet&
	getImageSet(
	    const uint64_t imageIndex,
	    const TemplateType templateType);

	/**
	 * @brief
	 * Format identification information about an ELFT implementation's
	 * ExtractionInterface.
	 *
	 * @param args
	 * Arguments parsed from command line (needed to retrieve configuration
	 * directory).
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked ELFT
	 * implementation.
	 */
	std::string
	getExtractionInterfaceIdentificationString(
	    const Arguments &args);

	/**
	 * @brief
	 * Format identification information about an ELFT implementation's
	 * SearchInterface.
	 *
	 * @param args
	 * Arguments parsed from command line (needed to retrieve configuration
	 * and reference database directories).
	 *
	 * @return
	 * Multiple "key = value" lines of information about the linked ELFT
	 * implementation.
	 */
	std::string
	getSearchInterfaceIdentificationString(
	    const Arguments &args);

	/**
	 * @brief
	 * Obtain the validation driver's usage string.
	 *
	 * @param name
	 * Name of the executable.
	 *
	 * @return
	 * Usage string.
	 */
	std::string
	getUsageString(
	    const std::string &name = "");

	/**
	 * @brief
	 * Generate single-file archive of templates with manifest.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @throw std::runtime_exception
	 * Error reading or writing to disk.
	 */
	void
	makeReferenceTemplateArchive(
	    const Arguments &args);

	/**
	 * @brief
	 * Create a template from one or more images.
	 *
	 * @param impl
	 * Pointer to ELFT extraction implementation.
	 * @param imageIndex
	 * Element index in the ImageSet vector.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * Entry for log file.
	 *
	 * @throw
	 * Error reading image or creating template.
	 */
	std::string
	performSingleCreate(
	    const std::shared_ptr<ExtractionInterface> impl,
	    const uint64_t imageIndex,
	    const Arguments &args);

	/**
	 * @brief
	 * Extract data from created template.
	 *
	 * @param impl
	 * Pointer to ELFT extraction implementation.
	 * @param templateType
	 * The type of template specified when the template was made during
	 * createTemplate().
	 * @param p
	 * Path to the template on disk.
	 *
	 * @return
	 * Entries for log file.
	 *
	 * @throw
	 * Error reading image or creating template.
	 */
	std::string
	performSingleExtractData(
	    const std::shared_ptr<ExtractionInterface> impl,
	    TemplateType templateType,
	    const std::filesystem::path &p);

	/**
	 * @brief
	 * Search a single probe template against a loaded reference database.
	 *
	 * @param impl
	 * Pointer to ELFT search implementation.
	 * @param identifier
	 * Identifier for `probeTemplate`.
	 * @param probeTemplate
	 * Template created by extraction interface to search against reference
	 * database.
	 * @param maxCandidates
	 * Maximum number of candidates to place in returned candidate list.
	 *
	 * @return
	 * A tuple containing the SearchResult and a string with entries for
	 * the candidates log file.
	 */
	std::tuple<ELFT::SearchResult, std::string>
	performSingleSearch(
	    const std::shared_ptr<SearchInterface> impl,
	    const std::string &identifier,
	    const std::vector<std::byte> &probeTemplate,
	    const uint16_t maxCandidates);

	/**
	 * @brief
	 * Extract correspondence for a single SearchResult.
	 *
	 * @param impl
	 * Pointer to ELFT search implementation.
	 * @param identifier
	 * Identifier for `probeTemplate`.
	 * @param probeTemplate
	 * Template created by extraction interface to search against reference
	 * database.
	 * @param searchResult
	 * SearchResult returned from SearchInterface::search for
	 * `probeTemplate` with the currently loaded reference database.
	 *
	 * @return
	 * Entries for log file.
	 */
	std::string
	performSingleSearchExtract(
	    const std::shared_ptr<SearchInterface> impl,
	    const std::string &identifier,
	    const std::vector<std::byte> &probeTemplate,
	    const SearchResult &searchResult);

	/**
	 * @brief
	 * Generate a random set of container indicies.
	 *
	 * @param size
	 * The number of items to be in the container.
	 * @param seed
	 * Randomization seed.
	 *
	 * @return
	 * Vector of size unique values.
	 */
	std::vector<uint64_t>
	randomizeIndicies(
	    const uint64_t size,
	    const uint64_t seed = std::random_device()());

	/**
	 * @brief
	 * Read a file from disk.
	 *
	 * @param pathName
	 * Path to file to read.
	 *
	 * @return
	 * Contents of pathName as a vector of bytes.
	 *
	 * @throw runtime_error
	 * Error reading from file.
	 */
	std::vector<std::byte>
	readFile(
	    const std::filesystem::path &pathName);

	/**
	 * @brief
	 * Read a file from disk.
	 *
	 * @param pathName
	 * Path to file to read.
	 *
	 * @return
	 * Contents of pathName as a vector of bytes.
	 *
	 * @throw runtime_error
	 * Error reading from file.
	 */
	std::vector<std::byte>
	readFile(
	    const std::string &pathName);

	/**
	 * @brief
	 * Have implementation create reference database on disk.
	 *
	 * @param impl
	 * Pointer to ELFT API implementation for extraction.
	 * @param args
	 * Arguments parsed from command line.
	 *
	 * @return
	 * EXIT_SUCCESS if `impl` was successful. EXIT_FAILURE otherwise.
	 */
	int
	runCreateReferenceDatabase(
	    std::shared_ptr<ExtractionInterface> impl,
	    const Arguments &args);

	/**
	 * @brief
	 * Run a set of template creations.
	 *
	 * @param impl
	 * Pointer to ELFT API implementation for extraction.
	 * @param indicies
	 * The indicies from Data::Latents or Data::References from which to
	 * create templates.
	 * @param args
	 * Arguments parsed from command line.
	 */
	void
	runExtractionCreate(
	    std::shared_ptr<ExtractionInterface> impl,
	    const std::vector<uint64_t> &indicies,
	    const Arguments &args);

	/**
	 * @brief
	 * Run a set of data extractions from templates created and output into
	 * template output directories this program knows about.
	 *
	 * @param impl
	 * Pointer to ELFT API implementation for extraction.
	 * @param indicies
	 * The indicies from Data::Latents or Data::References from which to
	 * create templates.
	 * @param args
	 * Arguments parsed from command line.
	 */
	void
	runExtractionExtractData(
	    std::shared_ptr<ExtractionInterface> impl,
	    const std::vector<uint64_t> &indicies,
	    const Arguments &args);

	/**
	 * @brief
	 * Run a set of probe template searches.
	 *
	 * @param impl
	 * Pointer to ELFT API implementation for searching.
	 * @param indicies
	 * The indicies from Data::Latents whose corresponding templates should
	 * be searched.
	 * @param args
	 * Arguments parsed from command line.
	 */
	void
	runSearch(
	    std::shared_ptr<SearchInterface> impl,
	    const std::vector<uint64_t> &indicies,
	    const Arguments &args);

	/**
	 * @brief
	 * Sanitize a message for printing in a log file.
	 *
	 * @param message
	 * Message to sanitize.
	 * @param escapeQuotes
	 * Whether or not to escape the " character.
	 * @param wrapInQuotes
	 * Whether or not to wrap result in ".
	 *
	 * @return
	 * Sanitized version of message that can be placed into a
	 * log file.
	 */
	std::string
	sanitizeMessage(
	    const std::string &message,
	    const bool escapeQuotes = true,
	    const bool wrapInQuotes = true);

	/**
	 * @brief
	 * Create multiple smaller sets from a large set.
	 *
	 * @param combinedSet
	 * Set of numbers to split.
	 * @param numSets
	 * Number of sets to create.
	 *
	 * @return
	 * Collection of numSets collections, created sequentially
	 * from combinedSet.
	 *
	 * @throw
	 * numSets > combinedSet.size()
	 */
	std::vector<std::vector<uint64_t>>
	splitSet(
	    const std::vector<uint64_t> &combinedSet,
	    const uint8_t numSets);

	/**
	 * @brief
	 * High-level spawn of tests of ELFT operations.
	 *
	 * @param args
	 * Arguments parsed from command line.
	 */
	void
	testOperation(
	    const Arguments &args);

	/**
	 * @brief
	 * Wait for forked children to exit.
	 *
	 * @param numChildren
	 * The expected number of children to exit.
	 */
	void
	waitForExit(
	    const uint8_t numChildren);

	/**
	 * @brief
	 * Write data to a file on disk.
	 *
	 * @param data
	 * Data to write.
	 * @param pathName
	 * Path at which to write data.
	 *
	 * @throw runtime_error
	 * Error writing to path.
	 */
	void
	writeFile(
	    const std::vector<std::byte> &data,
	    const std::string &pathName);

	/**
	 * @brief
	 * Parse command line arguments.
	 *
	 * @param argc
	 * argc from main().
	 * @param argv
	 * argv from main()
	 *
	 * @return
	 * Command line arguments parsed into an Argument.
	 *
	 * @throw std::exception
	 * Errors or inconsistencies when parsing arguments.
	 */
	Arguments
	parseArguments(
	    const int argc,
	    char * const argv[]);
}

#endif /* ELFT_VALIDATION_H_ */
