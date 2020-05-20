/**
 * @mainpage
 * @section Overview
 * This is the API that must be implemented to participate in the National
 * Institute of Standards and Technology (NIST)'s [Evaluation of Latent Friction
 * Ridge Technology (ELFT)](https://www.nist.gov/itl/iad/image-group/elft).
 *
 * @section Implementation
 * Two pure-virtual (abstract) classes called ELFT::ExtractionInterface and
 * ELFT::SearchInterface have been defined. Participants must implement all
 * methods of both classes in subclasses and submit the implementations in a
 * shared library. The name of the library must follow the requirements
 * outlined in the test plan and be identical to the required information
 * returned from ELFT::ExtractionInterface::getIdentification(). NIST's testing
 * application will link against the submitted library and instantiate instances
 * of the implementations with their respective getImplementation() functions
 * (ELFT::ExtractionInterface::getImplementation() and
 * ELFT::SearchInterface::getImplementation()).
 *
 * @section Contact
 * Additional information regarding ELFT can be received by emailing questions
 * to the test liaisons at elft@nist.gov.
 *
 * @section License
 * This software was developed at NIST by employees of the Federal Government
 * in the course of their official duties. Pursuant to title 17 Section 105 of
 * the United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef ELFT_H_
#define ELFT_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace ELFT
{
	/** Friction ridge impression types from ANSI/NIST-ITL 1-2011 (2015). */
	enum class Impression
	{
		PlainContact = 0,
		RolledContact = 1,
		Latent = 4,
		LiveScanSwipe = 8,
		PlainContactlessStationary = 24,
		RolledContactlessStationary = 25,
		Other = 28,
		Unknown = 29,
		RolledContactlessMoving = 41,
		PlainContactlessMoving = 42
	};

	/** Capture device codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class FrictionRidgeCaptureTechnology
	{
		Unknown = 0,
		ScannedInkOnPaper = 2,
		OpticalTIRBright = 3,
		OpticalDirect = 5,
		Capacitive = 9,
		Electroluminescent = 11,

		LatentImpression = 18,
		LatentLift = 22
	};

	/** Friction positions codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class FrictionRidgeGeneralizedPosition
	{
		UnknownFinger = 0,
		RightThumb = 1,
		RightIndex = 2,
		RightMiddle = 3,
		RightRing = 4,
		RightLittle = 5,
		LeftThumb = 6,
		LeftIndex = 7,
		LeftMiddle = 8,
		LeftRing = 9,
		LeftLittle = 10,
		RightExtraDigit = 16,
		LeftExtraDigit = 17,

		RightFour = 13,
		LeftFour = 14,
		RightAndLeftThumbs = 15,

		UnknownPalm = 20,
		RightFullPalm = 21,
		RightWritersPalm = 22,
		LeftFullPalm = 23,
		LeftWritersPalm = 24,
		RightLowerPalm = 25,
		RightUpperPalm = 26,
		LeftLowerPalm = 27,
		LeftUpperPalm = 28,
		RightPalmOther = 29,
		LeftPalmOther = 30,
		RightInterdigital = 31,
		RightThenar = 32,
		RightHypothenar = 33,
		LeftInterdigital = 34,
		LeftThenar = 35,
		LeftHypothenar = 36,
		RightGrasp = 37,
		LeftGrasp = 38,
		RightCarpalDeltaArea = 81,
		LeftCarpalDeltaArea = 82,
		RightFullPalmAndWritersPalm = 83,
		LeftFullPalmAndWritersPalm = 84,
		RightWristBracelet = 85,
		LeftWristBracelet = 86,

		UnknownFrictionRidge = 18,
		EJIOrTip = 19
	};

	/** EFS processing method codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class ProcessingMethod
	{
		Indanedione,
		BlackPowder,
		Other,
		Cyanoacrylate,
		Laser,
		RUVIS,
		StickysidePowder,
		Visual,
		WhitePowder
	};

	/**
	 * Classification of friction ridge structure.
	 *
	 * @note
	 * These enumerations map to ANSI/NIST-ITL 1-2011 Update:2015's PCT
	 * "General Class" codes from Table 44.
	 */
	enum class PatternClassification
	{
		Arch,
		Whorl,
		RightLoop,
		LeftLoop,
		Amputation,
		UnableToPrint,
		Unclassifiable,
		Scar,
		DissociatedRidges
	};

	/** EFS value assessment codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class ValueAssessment
	{
		Value,
		Limited,
		NoValue
	};

	/** EFS substrate codes from ANSI/NIST-ITL 1-2011 (2015). */
	enum class Substrate
	{
		Paper,
		PorousOther,

		Plastic,
		Glass,
		MetalPainted,
		MetalUnpainted,
		TapeAdhesiveSide,
		NonporousOther,

		PaperGlossy,
		SemiporousOther,

		Other,
		Unknown
	};

	/** Information about the result of calling an ELFT API function. */
	struct ReturnStatus
	{
		/** Possible outcomes when performing operations. */
		enum class Result
		{
			/** Successfully performed operation. */
			Success = 0,
			/** Failed to perform operation. */
			Failure
		};

		/** The result of the operation. */
		Result result{};
		/**
		 * Information about the result. Must match the regular
		 * expression `[[:graph:] ]*`.
		 */
		std::optional<std::string> message{};

		/**
		 * @return
		 * `true` if #result is Result::Success, `false` otherwise.
		 */
		explicit operator bool()
		    const
		    noexcept;
	};

	/** Data and metadata for an image. */
	struct Image
	{
		Image();

		/**
		 * @brief
		 * Image constructor.
		 *
		 * @param identifier
		 * An identifier for this image. Used to link Image to
		 * TemplateData and Correspondence.
		 * @param width
		 * Width of the image in pixels.
		 * @param height
		 * Height of the image in pixels.
		 * @param ppi
		 * Resolution of the image in pixels per inch.
		 * @param bpc
		 * Number of bits used by each color component (8 or 16).
		 * @param bpp
		 * Number of bits comprising a single pixel.
		 * @param pixels
		 * #width * #height * (#bpp / #bpc) bytes of image data, with
		 * `pixels.front()` representing the first byte of the top-left
		 * pixel, and `pixels.back()` representing the last byte of
		 * bottom-right pixel. It is decompressed little endian image
		 * data, canonically coded as defined in ISO/IEC 19794-4:2005,
		 * section 6.2.
		 *
		 * @note
		 * Number of color components is #bpp / #bpc and shall be either
		 * 1 (grayscale) or 3 (RGB).
		 */
		Image(
		    const uint8_t identifier,
		    const uint16_t width,
		    const uint16_t height,
		    const uint16_t ppi,
		    const uint8_t bpc,
		    const uint8_t bpp,
		    const std::vector<std::byte> &pixels);

		/**
		 * An identifier for this image. Used to link Image to EFS,
		 * TemplateData, and Correspondence.
		 */
		uint8_t identifier{};
		/** Width of the image. */
		uint16_t width{};
		/** Height of the image. */
		uint16_t height{};
		/** Resolution of the image in pixels per inch. */
		uint16_t ppi{};
		/** Number of bits used by each color component (8 or 16). */
		uint8_t bpc{};
		/** Number of bits comprising a single pixel. */
		uint8_t bpp{};
		/**
		 * @brief
		 * Raw pixel data of image.
		 *
		 * @details
		 * #width * #height * (#bpp / #bpc) bytes of image data, with
		 * `pixels.front()` representing the first byte of the top-left
		 * pixel, and `pixels.back()` representing the last byte of
		 * bottom-right pixel. It is decompressed little endian image
		 * data, canonically coded as defined in ISO/IEC 19794-4:2005,
		 *
		 * @note
		 * To pass pixels to a C-style array, invoke pixel's `data()`
		 * method (`pixels.data()`).
		 */
		std::vector<std::byte> pixels{};
	};

	/** Output from extracting features into a template .*/
	struct CreateTemplateResult
	{
		/** Result of extracting features and creating a template. */
		ReturnStatus status{};
		/** Contents of the template. */
		std::vector<std::byte> data{};
	};

	/** Pixel location in an image. */
	struct Coordinate
	{
		/** X coordinate in pixels. */
		uint32_t x{};
		/** Y coordinate in pixels. */
		uint32_t y{};

		/**
		 * @brief
		 * Coordinate constructor.
		 *
		 * @param x
		 * X coordinate in pixels.
		 * @param y
		 * Y coordinate in pixels.
		 */
		Coordinate(
		    const uint32_t x = {},
		    const uint32_t y = {});

		bool
		operator==(
		    const Coordinate &rhs)
		    const;

		bool
		operator<(
		    const Coordinate &rhs)
		    const;
	};

	/** Types of minutia. */
	enum class MinutiaType
	{
		RidgeEnding,
		Bifurcation,
		Other,
		Unknown
	};

	/** Friction ridge feature details. */
	struct Minutia
	{
		/** Location of the feature. */
		Coordinate coordinate{};
		/**
		 * Ridge direction of the feature, in degrees [0,359], following
		 * conventions from ANSI/NIST-ITL 1-2011 (2015) Field 9.331.
		 */
		uint16_t theta{};
		/** Type of feature */
		MinutiaType type{MinutiaType::Unknown};

		/**
		 * @brief
		 * Minutia constructor.
		 *
		 * @param coordinate
		 * Location of the feature.
		 * @param theta
		 * Ridge direction of the feature, in degrees [0,359], following
		 * conventions from ANSI/NIST-ITL 1-2011 (2015) Field 9.331.
		 * @param type
		 * Type of feature.
		 */
		Minutia(
		    const Coordinate &coordinate = {},
		    const uint16_t theta = {},
		    const MinutiaType type = MinutiaType::Unknown);
	};

	/** Location of identical features from two images. */
	struct Correspondence
	{
		/**
		 * Link to Image#identifier and/or EFS#identifier for
		 * reference. */
		uint8_t referenceInputIdentifier{};
		/** Location in the reference image of a probe image feature. */
		Minutia referenceMinutia{};
		/** Link to Image#identifier and/or EFS#identifier for probe. */
		uint8_t probeInputIdentifier{};
		/** Location in the probe image of a reference image feature. */
		Minutia probeMinutia{};

		/**
		 * @brief
		 * Correspondence constructor
		 *
		 * @param referenceInputIdentifier
		 * Link to Image#identifier and/or EFS#identifier for reference.
		 * @param referenceMinutia
		 * Location in the reference image of a probe image feature.
		 * @param probeInputIdentifier
		 * Link to Image#identifier and/or EFS#identifier for probe.
		 * @param probeMinutia
		 * Location in the probe image of a reference image feature.
		 */
		Correspondence(
		    const uint8_t referenceInputIdentifier = {},
		    const Minutia &referenceMinutia = {},
		    const uint8_t probeInputIdentifier = {},
		    const Minutia &probeMinutia = {});
	};

	/**
	 * Collection of ANSI/NIST-ITL 1-2011 (Update: 2015) Extended Feature
	 * Set fields understood by ELFT.
	 *
	 * @note
	 * All measurements and locations within the image SHALL be expressed in
	 * pixels, *not* units of 10 micrometers.
	 */
	struct EFS
	{
		/**
		 * An identifier for this set of data. Used to link EFS to
		 * Image, TemplateData, and Correspondence.
		 */
		uint8_t identifier{};

		/** Impression type of the depicted region. */
		Impression imp{Impression::Unknown};
		/** Capture technology that created this image. */
		FrictionRidgeCaptureTechnology frct{
		    FrictionRidgeCaptureTechnology::Unknown};
		/** Description of the depicted region. */
		FrictionRidgeGeneralizedPosition frgp{
		    FrictionRidgeGeneralizedPosition::UnknownFrictionRidge};

		/**
		 * Degrees to rotate image upright. Uncertainty is assumed to
		 * be +/- 15 degrees.
		 */
		std::optional<int16_t> orientation{};
		/** Methods used process the print. */
		std::optional<std::vector<ProcessingMethod>> lpm{};
		/** Examiner/algorithmic value assessment for identification. */
		std::optional<ValueAssessment> valueAssessment{};
		/** Substrate from which the print was developed. */
		std::optional<Substrate> lsb{};
		/** Observed pattern classification. */
		std::optional<PatternClassification> pat{};

		/**
		 * Image is known to be or may possibly be laterally reversed.
		 */
		std::optional<bool> plr{};
		/**
		 * Part or all of image is known to be or may possibly be
		 * tonally reversed.
		 */
		std::optional<bool> trv{};

		/**
		 * Core locations.
		 *
		 * @details
		 * Coordinate are relative to the bounding rectangle created
		 * by #roi, if supplied. Otherwise, they are relative to the
		 * source image.
		 */
		std::optional<std::vector<Coordinate>> cores{};
		/**
		 * Delta locations.
		 *
 		 * @details
		 * Coordinate are relative to the bounding rectangle created
		 * by #roi, if supplied. Otherwise, they are relative to the
		 * source image.
		 */
		std::optional<std::vector<Coordinate>> deltas{};
		/**
		 * Minutia locations.
		 *
 		 * @details
		 * Coordinate are relative to the bounding rectangle created
		 * by #roi, if supplied. Otherwise, they are relative to the
		 * source image.
 		 *
		 * @note
		 * NIST **strongly** discourages more than one Minutia at
		 * equivalent Coordinate. This can result in ambiguous
		 * Correspondence.
		 */
		std::optional<std::vector<Minutia>> minutia{};
		/** Closed convex polygon forming region of interest. */
		std::optional<std::vector<Coordinate>> roi{};
	};

	/**
	 * Information possibly stored in a template.
	 *
	 * @note
	 * If provided a multi-position image and applicable to the feature
	 * extraction algorithm, `roi` should be populated with segmentation
	 * coordinates and `frgp` should be set for each position.
	 */
	struct TemplateData
	{
		/** Link to Image#identifier and/or EFS#identifier. */
		uint8_t inputIdentifier{};

		/** Extended feature set data */
		std::optional<EFS> efs{};

		/** Quality of the image, [0-100]. */
		std::optional<uint8_t> imageQuality{};
	};

	/** Elements of a candidate list. */
	struct Candidate
	{
		/** Identifier of the sample in the reference database. */
		std::string identifier{};
		/** Most localized position in the identifier. */
		FrictionRidgeGeneralizedPosition frgp{};
		/** Quantification of probe's similarity to reference sample. */
		double similarity{};

		/**
		 * @brief
		 * Candidate constructor.
		 *
		 * @param identifier
		 * Identifier of the sample in the reference database.
		 * @param frgp
		 * Most localized position in the identifier.
		 * @param similarity
		 * Quantification of probe's similarity to reference sample.
		 */
		Candidate(
		    const std::string &identifier = {},
		    const FrictionRidgeGeneralizedPosition frgp = {},
		    const double similarity = {});

		bool
		operator==(
		    const Candidate &rhs)
		    const;

		bool
		operator<(
		    const Candidate &rhs)
		    const;
	};

	/** The results of a searching a database. */
	struct SearchResult
	{
		/**
		 * Status of searching reference database and assembling
		 * candidate list.
		 */
		ReturnStatus status{};
		/**
		 * Best guess on if #candidateList contains an identification.
		 */
		bool decision{};
		/** List of Candidate most similar to the probe. */
		std::vector<Candidate> candidateList{};
	};

	/** Types of templates created by this interface. */
	enum class TemplateType
	{
		/** Template to be used as probe in a search. */
		Probe,
		/** Template to be added to a reference database. */
		Reference
	};

	/** Identifying details about algorithm components for documentation. */
	struct ProductIdentifier
	{
		/** CBEFF information registered with and assigned by IBIA. */
		struct CBEFFIdentifier
		{
			/** CBEFF Product Owner of the product. */
			uint16_t owner{};
			/** CBEFF Algorithm Identifier of the product. */
			std::optional<uint16_t> algorithm{};
		};

		/**
		 * Non-infringing marketing name of the product. Case sensitive.
		 * Must match the regular expression `[[:graph:] ]*`.
		 */
		std::optional<std::string> marketing{};
		/** CBEFF information about the product. */
		std::optional<CBEFFIdentifier> cbeff{};
	};

	/** Interface for feature extraction implemented by participant. */
	class ExtractionInterface
	{
	public:
		/**
		 * Identifying information about this submission that will be
		 * included in reports.
		 */
		struct SubmissionIdentification
		{
			SubmissionIdentification();

			/**
			 * @brief
			 * SubmissionIdentification constructor.
			 *
			 * @param versionNumber
			 * Version number of this submission. Required to be
			 * unique for each new submission.
			 * @param libraryIdentifier
			 * Non-infringing identifier of this submission. Should
			 * be the same for all submissions. Case sensitive.
			 * Must match the regular expression `[:alnum:]+`.
			 * @param exemplarAlgorithmIdentifier
			 * Information about the exemplar feature extraction
			 * algorithm in this submission.
			 * @param latentAlgorithmIdentifier
			 * Information about the latent feature extraction
			 * algorithm in this submission.
			 *
			 * @note
			 * The name of the core library submitted for
			 * evaluation shall be
			 * "libelft_<libraryIdentifier>_<versionNumber (capital
			 * hex)>.so". Refer to the test plan for more
			 * information.
			 */
			SubmissionIdentification(
			    const uint16_t versionNumber,
			    const std::string &libraryIdentifier,
			    const std::optional<ProductIdentifier>
			        &exemplarAlgorithmIdentifier = {},
			    const std::optional<ProductIdentifier>
			        &latentAlgorithmIdentifier = {});

			/**
			 * Version number of this submission. Required to be
			 * unique for each new submission.
			 */
			uint16_t versionNumber{};
			/**
			 * Non-infringing identifier of this submission. Should
			 * be the same for all submissions from an organization.
			 * Case sensitive. Must match the regular expression
			 * `[:alnum:]+`.
			 */
			std::string libraryIdentifier{};

			/**
			 * Information about the exemplar feature extraction
			 * algorithm in this submission.
			 */
			std::optional<ProductIdentifier>
			    exemplarAlgorithmIdentifier{};
			/**
			 * Information about the latent feature extraction
			 * algorithm in this submission.
			 */
			std::optional<ProductIdentifier>
			    latentAlgorithmIdentifier{};
		};

		/**
		 * @brief
		 * Obtain identification and version information for the
		 * extraction portion of this submission.
		 *
		 * @return
		 * SubmissionIdentification populated with information used
		 * to identify the feature extraction algorithms in reports.
		 *
		 * @note
		 * This method shall return instantly.
		 */
		virtual
		SubmissionIdentification
		getIdentification()
		    const = 0;

		/**
		 * @brief
		 * Extract features from one or more images and encode them into
		 * a template.
		 *
		 * @param templateType
		 * Operation where this template will be used in future
		 * searches.
		 * @param identifier
		 * Unique identifier used to identify the returned template
		 * in future *search* operations (e.g., Candidate#identifier).
		 * @param samples
		 * One or more biometric samples to be considered and encoded
		 * into a template.
		 *
		 * @return
		 * A single CreateTemplateResult, which contains information
		 * about the result of the operation and a single template.
		 *
		 * @note
		 * This method must return in <= 500 * `samples.size()`
		 * microseconds, on average, as measured on a fixed subset of
		 * data.
		 *
		 * @note
		 * The value of the returned CreateTemplateResult#data will only
		 * be recorded if CreateTemplateResult's ReturnStatus#result is
		 * ReturnStatus::Result::Success. On ReturnStatus::Result::
		 * Failure, subsequent searches will automatically increase
		 * false negative identification rate.
		 */
		virtual
		CreateTemplateResult
		createTemplate(
		    const TemplateType templateType,
		    const std::string &identifier,
		    const std::vector<std::tuple<
		        std::optional<Image>, std::optional<EFS>>> &samples)
		    const = 0;

		/**
		 * @brief
		 * Extract information contained within a template.
		 *
		 * @param templateType
		 * templateType passed to `createTemplate()`.
		 * @param templateResult
		 * Object returned from `createTemplate()`.
		 *
		 * @return
		 * One or more TemplateData describing the contents of
		 * CreateTemplateResult#data from `templateResult`. If
		 * CreateTemplateResult#data contains information separated by
		 * position (e.g., when provided a multi-position image) or
		 * multiple views of the same image (e.g., a compact and verbose
		 * template), there can be multiple TemplateData returned.
		 *
		 * @note
		 * You must implement this method to compile, but providing the
		 * requested information is optional. If provided, information
		 * may help in debugging as well as inform future NIST analysis.
		 *
		 * @note
		 * You should not return information that was provided in
		 * createTemplate(). For instance, if examiner Minutia was
		 * provided, EFS#minutia should be left `std::nullopt`. However,
		 * if you discovered _different_ Minutia, they should be
		 * returned.
		 *
		 * @note
		 * The ReturnStatus member of CreateTemplateResult is not
		 * guaranteed to be populated with ReturnStatus#message and
		 * should not be consulted.
		 */
		virtual
		std::optional<std::vector<TemplateData>>
		extractTemplateData(
		    const TemplateType templateType,
		    const CreateTemplateResult &templateResult)
		    const = 0;

		/**
		 * @brief
		 * Create a reference database on the filesystem.
		 *
		 * @param referenceTemplates
		 * One or more templates returned from createTemplate() with
		 * a `templateType` of TemplateType::Reference.
		 * @param databaseDirectory
		 * Entry to a read/write directory where the reference database
		 * shall be written.
		 * @param maxSize
		 * The maximum number of bytes of storage available to write.
		 *
		 * @return
		 * Information about the result of executing the method.
		 *
		 * @note
		 * This method may use more than one thread.
		 *
		 * @note
		 * This method must return in <= 10 *
		 * `referenceTemplates.size()` microseconds.
		 */
		virtual
		ReturnStatus
		createReferenceDatabase(
		    const std::vector<std::vector<std::byte>>
		        &referenceTemplates,
		    const std::filesystem::path &databaseDirectory,
		    const uint64_t maxSize)
		    const = 0;

		/**************************************************************/

		/**
		 * @brief
		 * Obtain a managed pointer to an object implementing
		 * ExtractionInterface.
		 *
		 * @param configurationDirectory
		 * Read-only directory populated with configuration files
		 * provided in validation.
		 *
		 * @return
		 * Shared pointer to an instance of ExtractionInterface
		 * containing the participant's code to perform extraction
		 * operations.
		 *
		 * @note
		 * A possible implementation might be:
		 * `return (std::make_shared<ExtractionImplementation>(
		 *      configurationDirectory));`
		 *
		 * @note
		 * This method shall return in <= 10 seconds.
		 */
		static
		std::shared_ptr<ExtractionInterface>
		getImplementation(
		    const std::filesystem::path &configurationDirectory);

		/** @cond SUPPRESS_FROM_DOXYGEN */
		/** Suppress copying polymorphic class (C.63). */
		ExtractionInterface(const ExtractionInterface&) = delete;
		/** Suppress copying polymorphic class (C.63). */
		ExtractionInterface& operator=(const ExtractionInterface&) =
		    delete;
		/** @endcond */

		ExtractionInterface();
		virtual ~ExtractionInterface();
	};

	/** Interface for database search implemented by participant. */
	class SearchInterface
	{
	public:
		/**
		 * @brief
		 * Obtain identification and version information for the
		 * search portion of this submission.
		 *
		 * @return
		 * ProductIdentifier populated with information used to identify
		 * the search algorithm in reports.
		 *
		 * @note
		 * This method shall return instantly.
		 */
		virtual
		std::optional<ProductIdentifier>
		getIdentification()
		    const = 0;

		/**
		 * @brief
		 * Determine if an identifier is in the reference database.
		 *
		 * @param identifier
		 * Identifier to check.
		 *
		 * @return
		 * A tuple whose first member is the result of executing the
		 * operation, and whose second member is `true` if identifier is
		 * represented in the reference database, and `false` otherwise.
		 *
		 * @note
		 * This method must return in <= 5 seconds.
		 *
		 * @note
		 * This method need not be threadsafe. It may use more than one
		 * thread.
		 */
		virtual
		std::tuple<ReturnStatus, bool>
		exists(
		    const std::string &identifier)
		    const = 0;

		/**
		 * @brief
		 * Insert or update an identifier in a loaded reference
		 * database.
		 * @details
		 * This method should limit the amount of I/O and processing
		 * necessary, as indicated by the runtime limitation noted
		 * below.
		 *
		 * @param identifier
		 * Identifier to add or update.
		 * @param referenceTemplate
		 * A template returned from
		 * ExtractionInterface::createTemplate() with a `templateType`
		 * of TemplateType::Reference.
		 *
		 * @return
		 * Information about the result of executing the method.
		 *
		 * @note
		 * If `identifier` already exists in the enrollment database,
		 * this method should "merge" data that already exists in the
		 * database with `referenceTemplate` before replacing the
		 * entry in the database.
		 *
		 * @note
		 * This method must return in <= 5 seconds.
		 *
 		 * @note
		 * This method need not be threadsafe. It may use more than one
		 * thread.
		 */
		virtual
		ReturnStatus
		insert(
		    const std::string &identifier,
		    const std::vector<std::byte> &referenceTemplate) = 0;

		/**
		 * @brief
		 * Remove an identifier from a loaded reference database.
		 * @details
		 * This method should limit the amount of I/O and processing
		 * necessary, as indicated by the runtime limitation noted
		 * below.
		 *
		 * @param identifier
		 * Identifier to remove.
		 *
		 * @return
		 * Information about the result of executing the method.
		 *
		 * @note
		 * This method must return in <= 5 seconds.
		 *
 		 * @note
		 * This method need not be threadsafe. It may use more than one
		 * thread.
		 */
		virtual
		ReturnStatus
		remove(
		    const std::string &identifier) = 0;

		/**************************************************************/

		/**
		 * @brief
		 * Search the reference database for the samples represented in
		 * `probeTemplate`.
		 *
		 * @param probeTemplate
		 * Object returned from `createTemplate()` with `templateType`
		 * of `Probe`.
		 * @param maxCandidates
		 * The maximum number of Candidate to return.
		 *
		 * @return
		 * A SearchResult object containing information on if this task
		 * was able to be completed and a list of less than or equal to
		 * `maxCandidates` Candidate.
		 *
		 * @note
		 * SearchResult.candidateList will be sorted by descending
		 * `similarity` upon return from this method using
		 * std::stable_sort().
		 *
		 * @note
		 * If provided a probe template that contains comes from
		 * multiple regions, Candidate.frgp will be ignored.
		 *
		 * @note
		 * Candidate.frgp shall be the most localized region where the
		 * match was made to be considered correct. For instance, if
		 * a latent right hypothenar region was searched, it is
		 * incorrect to return right full palm or right lower palm, even
		 * if it is from the correct identifier, regardless of the image
		 * types provided when creating the reference template.
		 *
		 * @note
		 * This method must return in <= 300 * `number of database
		 * identifiers` microseconds, on average, as measured on a fixed
		 * subset of data.
		 */
		virtual
		SearchResult
		search(
		    const std::vector<std::byte> &probeTemplate,
		    const uint16_t maxCandidates)
		    const = 0;

		/**
		 * @brief
		 * Extract pairs of corresponding minutia between probe template
		 * and reference template.
		 *
		 * @param probeTemplate
		 * Probe template sent to searchReferences().
		 * @param searchResult
		 * Object returned from searchReferences().
		 *
		 * @return
		 * A vector the length of `searchResult.candidateList.size()`,
		 * where each entry is the collection of corresponding minutia
		 * points between `probeTemplate` and the reference template of
		 * the Candidate at the same position as `searchResult`'s
		 * SearchResult.candidateList.
		 *
		 * @note
		 * Minutia must align with minutia returned from
		 * ExtractionInterface::extractTemplateData() for the given
		 * identifier + position pair.
		 *
		 * @note
		 * You must implement this method to compile, but providing
		 * the requested information is optional. If provided,
		 * information may help in debugging, as well as informing
		 * future NIST analysis.
		 *
		 * @note
		 * `searchResult` is **not guaranteed** to be the identical
		 * object returned from search(). Specifically, ordering of
		 * `searchResult.candidateList` may have changed (e.g., sorted
		 * by descending `similarity`) and the ReturnStatus member is
		 * not guaranteed to populated with ReturnStatus#message.
		 */
		virtual
		std::optional<std::vector<std::vector<Correspondence>>>
		extractCorrespondence(
		    const std::vector<std::byte> &probeTemplate,
		    const SearchResult &searchResult)
		    const = 0;

		/**************************************************************/

		/**
		 * @brief
		 * Obtain a managed pointer to an object implementing
		 * SearchInterface.
		 *
		 * @param configurationDirectory
		 * Read-only directory populated with configuration files
		 * provided in validation.
		 * @param databaseDirectory
		 * Read-only directory populated with files written in
		 * ExtractionInterface::createReferenceDatabase().
		 *
		 * @return
		 * Shared pointer to an instance of SearchInterface containing
		 * the participant's code to perform search operations.
		 *
		 * @note
		 * A possible implementation might be:
		 * `return (std::make_shared<SearchImplementation>(
		 *      configurationDirectory, databaseDirectory));`
		 *
		 * @note
		 * This method shall return in <= 5 seconds.
		 */
		static
		std::shared_ptr<SearchInterface>
		getImplementation(
		    const std::filesystem::path &configurationDirectory,
		    const std::filesystem::path &databaseDirectory);

		/** @cond SUPPRESS_FROM_DOXYGEN */
		/** Suppress copying polymorphic class (C.63). */
		SearchInterface(const SearchInterface&) = delete;
		/** Suppress copying polymorphic class (C.63). */
		SearchInterface& operator=(const SearchInterface&) = delete;
		/** @endcond */

		SearchInterface();
		virtual ~SearchInterface();
	};

	/*
	 * API versioning.
	 *
	 * NIST code will extern the version number symbols. Participant code
	 * shall compile them into their core library.
	 */
	#ifdef NIST_EXTERN_API_VERSION
	/** API major version number. */
	extern uint16_t API_MAJOR_VERSION;
	/** API minor version number. */
	extern uint16_t API_MINOR_VERSION;
	/** API patch version number. */
	extern uint16_t API_PATCH_VERSION;
	#else /* NIST_EXTERN_API_VERSION */
	/** API major version number. */
	uint16_t API_MAJOR_VERSION{0};
	/** API minor version number. */
	uint16_t API_MINOR_VERSION{0};
	/** API patch version number. */
	uint16_t API_PATCH_VERSION{1};
	#endif /* NIST_EXTERN_API_VERSION */
}

#endif /* ELFT_H_ */
