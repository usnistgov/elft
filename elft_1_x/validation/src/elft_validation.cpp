/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <sys/wait.h>

#include <getopt.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <system_error>
#include <thread>
#include <variant>

#include <elft.h>
#include <elft_validation.h>
#include <elft_validation_data.h>
#include <elft_validation_utils.h>

int
ELFT::Validation::dispatchOperation(
    const ELFT::Validation::Arguments &args)
{
	int rv{EXIT_FAILURE};

	switch (args.operation.value_or(Operation::Usage)) {
	case Operation::Usage:
		std::cout << getUsageString("elft_validation") << '\n';
		rv = EXIT_SUCCESS;
		break;
	case Operation::Identify:
		try {
			std::cout << getExtractionInterfaceIdentificationString(
			    args) << '\n';
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "ExtractionInterface::"
			    "getIdentification(): " << e.what() << '\n';
		} catch (...) {
			std::cerr << "ExtractionInterface::"
			    "getIdentification(): Non-standard exception\n";
		}
		break;
	case Operation::IdentifySearch:
		try {
			std::cout << getSearchInterfaceIdentificationString(
			    args) << '\n';
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "SearchInterface::getIdentification(): " <<
			    e.what() << '\n';
		} catch (...) {
			std::cerr << "SearchInterface::getIdentification(): "
			    "Non-standard exception\n";
		}
		break;
	case Operation::Extract:
		try {
			testOperation(args);
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "Extract: " << e.what() << '\n';
		} catch (...) {
			std::cerr << "Extract: Non-standard exception\n";
		}
		break;
	case Operation::CreateReferenceDatabase:
		try {
			const auto impl = ELFT::ExtractionInterface::
			    getImplementation(args.configDir);
			rv = runCreateReferenceDatabase(impl, args);
		} catch (const std::exception &e) {
			std::cerr << "CreateReferenceDatabase: " << e.what() <<
			    '\n';
		} catch (...) {
			std::cerr << "CreateReferenceDatabase: Non-standard "
			    "exception\n";
		}
		break;
	case Operation::Search:
		try {
			testOperation(args);
			rv = EXIT_SUCCESS;
		} catch (const std::exception &e) {
			std::cerr << "Search: " << e.what() << '\n';
		} catch (...) {
			std::cerr << "Search: Non-standard exception\n";
		}
		break;
	}

	return (rv);
}

std::string
ELFT::Validation::getExtractionInterfaceIdentificationString(
    const Arguments &args)
{
	const auto id = ELFT::ExtractionInterface::getImplementation(
	    args.configDir)->getIdentification();

	std::stringstream ss{};
	ss << "Identifier = " << id.libraryIdentifier << '\n' <<
	    "Version = 0x" << std::setw(4) << std::setfill('0') << std::hex <<
	    std::uppercase << id.versionNumber;
	ss << std::dec << std::nouppercase << '\n';

	if (id.exemplarAlgorithmIdentifier) {
		ss << "Exemplar Feature Extraction Algorithm Marketing "
		    "Identifier =";
		if (id.exemplarAlgorithmIdentifier->marketing)
			ss << ' ' << *id.exemplarAlgorithmIdentifier->marketing;
		ss << '\n';

		if (id.exemplarAlgorithmIdentifier->cbeff) {
			ss << "Exemplar Feature Extraction Algorithm CBEFF "
			    "Owner =";
			ss << " 0x" << std::setw(4) << std::setfill('0') <<
			    std::hex << std::uppercase <<
			    id.exemplarAlgorithmIdentifier->cbeff->owner <<
			    std::dec << std::nouppercase;
			ss << '\n';

			if (id.exemplarAlgorithmIdentifier->cbeff->algorithm) {
				ss << "Exemplar Feature Extraction Algorithm "
				    "CBEFF Identifier =";
				ss << " 0x" << std::setw(4) <<
				    std::setfill('0') <<
				    std::hex << std::uppercase <<
				    *id.exemplarAlgorithmIdentifier->cbeff->
				    algorithm << std::dec << std::nouppercase;
			} else {
				ss << "Exemplar Feature Extraction Algorithm "
				    "CBEFF Identifier =";
			}
			ss << '\n';
		} else {
			ss << "Exemplar Feature Extraction Algorithm CBEFF "
			    "Owner =\n";
			ss << "Exemplar Feature Extraction Algorithm CBEFF "
			    "Identifier =\n";
		}
	}

	if (id.latentAlgorithmIdentifier) {
		ss << "Latent Feature Extraction Algorithm Marketing "
		    "Identifier =";
		if (id.latentAlgorithmIdentifier->marketing)
			ss << ' ' << *id.latentAlgorithmIdentifier->marketing;
		ss << '\n';

		if (id.latentAlgorithmIdentifier->cbeff) {
			ss << "Latent Feature Extraction Algorithm CBEFF "
			    "Owner =";
			ss << " 0x" << std::setw(4) << std::setfill('0') <<
			    std::hex << std::uppercase <<
			    id.latentAlgorithmIdentifier->cbeff->owner <<
			    std::dec << std::nouppercase;
			ss << '\n';

			if (id.latentAlgorithmIdentifier->cbeff->algorithm) {
				ss << "Latent Feature Extraction Algorithm "
				    "CBEFF Identifier =";
				ss << " 0x" << std::setw(4) <<
				    std::setfill('0') <<
				    std::hex << std::uppercase <<
				    *id.latentAlgorithmIdentifier->cbeff->
				    algorithm << std::dec << std::nouppercase;
			} else {
				ss << "Latent Feature Extraction Algorithm "
				    "CBEFF Identifier =";
			}
		} else {
			ss << "Latent Feature Extraction Algorithm CBEFF "
			    "Owner =\n";
			ss << "Latent Feature Extraction Algorithm CBEFF "
			    "Identifier =";
		}
	}

	return (ss.str());
}

const ELFT::Validation::Data::ImageSet&
ELFT::Validation::getImageSet(
    const uint64_t imageIndex,
    const TemplateType templateType)
{
	switch(templateType) {
	case TemplateType::Probe:
		return Data::Probes.at(imageIndex);
	case TemplateType::Reference:
		return Data::References.at(imageIndex);
	default:
		throw std::runtime_error{"getImageSet() called with unknown "
		    "TemplateType"};
	}
}

std::string
ELFT::Validation::getSearchInterfaceIdentificationString(
    const Arguments &args)
{
	const auto id = ELFT::SearchInterface::getImplementation(
	    args.configDir, args.dbDir)->getIdentification();

	std::stringstream ss{};
	if (id) {
		ss << "Search Algorithm Marketing Identifier =";
		if (id->marketing)
			ss << ' ' << *id->marketing;
		ss << '\n';

		ss << "Search Algorithm CBEFF Owner =";
		if (id->cbeff) {
			ss << " 0x" << std::setw(4) << std::setfill('0') <<
			    std::hex << std::uppercase <<
			    id->cbeff->owner << std::dec << std::nouppercase;
		}
		ss << '\n';

		ss << "Search Algorithm CBEFF Identifier =";
		if (id->cbeff->algorithm) {
			ss << " 0x" << std::setw(4) <<
			    std::setfill('0') << std::hex << std::uppercase <<
			    *id->cbeff->algorithm << std::dec <<
			    std::nouppercase;
		}
	} else {
		ss << "Search Algorithm Marketing Identifier =\n";
		ss << "Search Algorithm CBEFF Owner =\n";
		ss << "Search Algorithm CBEFF Identifier =";
	}

	return (ss.str());
}

std::string
ELFT::Validation::getUsageString(
    const std::string &name)
{
	static const std::string usagePrompt{"Usage:"};
	const std::string prefix(usagePrompt.length() + 4, ' ');

	std::stringstream ss{};
	ss << usagePrompt << ' ' << name << " ...\n";
	ss << prefix << "# Identify (ExtractionInterface)\n" << prefix <<
	     "-i -z <configDir>\n";

	ss << '\n';

	ss << prefix << "# createTemplate() + extractTemplateData()\n" <<
	    prefix << "-e <probe|reference> -z <configDir> [-o <outputDir>] "
	   "[-a image_dir]\n" << prefix << "[-r random_seed] [-f num_procs]\n";

	ss << '\n';

	ss << prefix << "# createReferenceDatabase()\n" << prefix <<
	    "-c -d <referenceDir> -z <configDir> [-o <outputDir>] "
	    "[-m max_size]\n";

	ss << '\n';

	ss << prefix << "# Identify (SearchInterface)\n" << prefix <<
	    "-j -d <referenceDir> -z <configDir>\n";

	ss << '\n';

	ss << prefix << "# search() + extractCorrespondence()\n" << prefix <<
	    "-s -d <referenceDir> -z <configDir> [-o <outputDir>] "
	    "[-r random_seed]\n" << prefix <<
	    "[-m max_candidates] [-f num_procs]\n";

	ss << '\n';

	ss << prefix << "# Database modification operations\n" << prefix <<
	    "-t -d <referenceDir> -z <configDir> [-o <outputDir>]";

	return (ss.str());
}

void
ELFT::Validation::makeReferenceTemplateArchive(
    const ELFT::Validation::Arguments &args)
{
	const auto dir = args.outputDir / Data::getTemplateDir(
	    TemplateType::Reference);

	if (std::filesystem::exists(dir / Data::TemplateArchiveArchiveName))
		throw std::runtime_error{
		    (dir / Data::TemplateArchiveArchiveName).string() + " "
		        "already exists"};
	std::ofstream archive{dir / Data::TemplateArchiveArchiveName,
	    std::ios_base::out | std::ios_base::binary |
	    std::ios_base::app};
	if (!archive)
		throw std::runtime_error{"Could not open " +
		    (dir / Data::TemplateArchiveArchiveName).string()};

	if (std::filesystem::exists(dir / Data::TemplateArchiveManifestName))
		throw std::runtime_error{
		    (dir / Data::TemplateArchiveManifestName).string() + " "
		        "already exists"};
	std::ofstream manifest{dir / Data::TemplateArchiveManifestName,
	    std::ios_base::out | std::ios_base::binary |
	    std::ios_base::app};
	if (!manifest)
		throw std::runtime_error{"Could not open " +
		    (dir / Data::TemplateArchiveManifestName).string()};

	for (const auto &entry : std::filesystem::recursive_directory_iterator(
	    dir)) {
		if ((entry.path().extension() != Data::TemplateSuffix) ||
		    !entry.is_regular_file())
			continue;

		std::ifstream tmpl{entry.path(), std::ios_base::in |
		    std::ios_base::binary | std::ios_base::ate};
		if (!tmpl)
			throw std::runtime_error{"Could not open " +
			    entry.path().string()};

		const auto tmplDataSize = tmpl.tellg();
		if (!tmpl)
			throw std::runtime_error{"Could not get size of " +
			    entry.path().string()};

		tmpl.seekg(0);
		if (!tmpl)
			throw std::runtime_error{"Could not rewind " +
			    entry.path().string()};

		std::vector<std::byte> tmplData(
		    static_cast<std::vector<std::byte>::size_type>(
		    tmplDataSize));
		tmpl.read(reinterpret_cast<char*>(tmplData.data()),
		    tmplDataSize);
		if (!tmpl)
			throw std::runtime_error{"Could not read " +
			    entry.path().string()};

		const auto currentOffset = archive.tellp();
		if (!archive)
			throw std::runtime_error{"Could not get current "
			    "offset from " +
			    (dir / Data::TemplateArchiveArchiveName).string()};

		archive.write(reinterpret_cast<char*>(tmplData.data()),
		    tmplDataSize);
		if (!archive)
			throw std::runtime_error{"Could not write " +
			    (dir / Data::TemplateArchiveArchiveName).string()};

		manifest << entry.path().filename().string() << ' ' <<
		    tmplDataSize << ' ' << currentOffset << '\n';
		if (!manifest)
			throw std::runtime_error{"Could not write " +
			    (dir / Data::TemplateArchiveManifestName).string()};
	}
}

ELFT::Validation::Arguments
ELFT::Validation::parseArguments(
    const int argc,
    char * const argv[])
{
	static const char options[] {"a:cd:e:f:ijm:o:r:sz:"};
	Validation::Arguments args{};

	int c{};
	while ((c = getopt(argc, argv, options)) != -1) {
		switch (c) {
		case 'a':	/* Image directory */
			args.imageDir = optarg;
			break;
		case 'c':	/* Create reference database */
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};
			args.operation = Operation::CreateReferenceDatabase;
			break;
		case 'd':	/* Enrollment database directory */
			args.dbDir = optarg;
			break;
		case 'e':	/* Extract */
		{
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};
			args.operation = Operation::Extract;

			std::string type{optarg};
			type = lower(type);
			if (type == "probe")
				args.templateType = TemplateType::Probe;
			else if (type == "reference")
				args.templateType = TemplateType::Reference;
			else
				throw std::invalid_argument{"Incorrect type of "
				    "template. Argument to -e must be "
				    "\"probe\" or \"reference.\""};
			break;
		}
		case 'f': {	/* Number of processes */
			try {
				args.numProcs = static_cast<uint8_t>(
				    std::stoul(optarg));
			} catch (const std::exception&) {
				throw std::invalid_argument{"Number of "
				    "processes (-f): an error occurred when "
				    "parsing \"" + std::string(optarg) + "\""};
			}

			/* Need to allow 2 procs, even if only 1 CPU (VM) */
			const auto threadCount = std::max(
			    static_cast<unsigned int>(2), std::thread::
			    hardware_concurrency());
			if ((threadCount == 0 && args.numProcs > 4) ||
			    (args.numProcs > threadCount))
				throw std::invalid_argument{"Number of "
				    "processes (-f): Asked to spawn " +
				    ts(args.numProcs) + " processes, but "
				    "refusing"};
			break;
		}
		case 'i':	/* ExtractionInterface identification */
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};
			args.operation = Operation::Identify;
			break;
		case 'j':	/* SearchInterface identification */
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};
			args.operation = Operation::IdentifySearch;
			break;
		case 'm':	/* Max {candidate list, db} size */
			try {
				args.maximum = std::stoull(optarg);
			} catch (const std::exception&) {
				throw std::invalid_argument{"Maximum size "
				    "(-m): an error occurred when parsing \"" +
				    std::string(optarg) + "\""};
			}
			break;
		case 'o':	/* Output directory */
			args.outputDir = optarg;
			break;
		case 'r':	/* Random seed */
			try {
				args.randomSeed = std::stoull(optarg);
			} catch (const std::exception&) {
				throw std::invalid_argument{"Random seed (-r): "
				    "an error occurred when parsing \"" +
				    std::string(optarg) + "\""};
			}
			break;
		case 's':	/* Search */
			if (args.operation)
				throw std::logic_error{"Multiple operations "
				    "specified"};
			args.operation = Operation::Search;
			break;
		case 'z':	/* Config dir */
			args.configDir = optarg;
			break;
		}
	}

	if (!args.operation)
		args.operation = Operation::Usage;

	if (args.configDir.empty() && (args.operation != Operation::Usage))
		throw std::invalid_argument{"Must provide path to "
		     "configuration directory"};

	if (args.dbDir.empty() && (
	    (args.operation == Operation::IdentifySearch) ||
	    (args.operation == Operation::CreateReferenceDatabase) ||
	    (args.operation == Operation::Search)))
		throw std::invalid_argument{"Must provide path to reference "
		    "database"};

	if (args.maximum == 0) {
		if (args.operation == Operation::CreateReferenceDatabase)
			args.maximum = 100000000;
		else if (args.operation == Operation::Search)
			args.maximum = 100;
	} else if (args.operation == Operation::Search) {
		if (args.maximum > UINT16_MAX)
			throw std::invalid_argument{"Maximum number of "
			    "candidates (-m) is " + ts(UINT16_MAX)};
	}

	return {args};
}

std::vector<uint64_t>
ELFT::Validation::randomizeIndicies(
    const uint64_t size,
    const uint64_t seed)
{
	std::vector<uint64_t> indicies(size);
	std::iota(indicies.begin(), indicies.end(), 0);
	std::shuffle(indicies.begin(), indicies.end(), std::mt19937_64(seed));
	return (indicies);
}

std::vector<std::byte>
ELFT::Validation::readFile(
    const std::filesystem::path &pathName)
{
	return (readFile(pathName.string()));
}

std::vector<std::byte>
ELFT::Validation::readFile(
    const std::string &pathName)
{
	std::ifstream file{pathName,
	    std::ifstream::ate | std::ifstream::binary};
	if (!file)
		throw std::runtime_error{"Could not open " + pathName};
	file.unsetf(std::ifstream::skipws);

	const auto size = file.tellg();
	if (size == -1)
		throw std::runtime_error{"Could not open " + pathName};

	std::vector<std::byte> buf{};
	buf.resize(static_cast<std::vector<std::byte>::size_type>(size));
	file.seekg(std::ifstream::beg);
	file.read(reinterpret_cast<char*>(buf.data()), size);

	return (buf);
}

int
ELFT::Validation::runCreateReferenceDatabase(
    std::shared_ptr<ExtractionInterface> impl,
    const Arguments &args)
{
	std::filesystem::create_directories(args.dbDir);
	std::filesystem::permissions(args.dbDir,
	    std::filesystem::perms::owner_all |
	    std::filesystem::perms::group_all);

	const auto archivePath = args.outputDir / Data::getTemplateDir(
	    TemplateType::Reference) / Data::TemplateArchiveArchiveName;
	const auto manifestPath = args.outputDir / Data::getTemplateDir(
	    TemplateType::Reference) / Data::TemplateArchiveManifestName;
	if (!std::filesystem::exists(archivePath) ||
	    !std::filesystem::exists(manifestPath))
		throw std::runtime_error{"Member of TemplateArchive does not"
		    "exist"};
	TemplateArchive referenceTemplates{archivePath, manifestPath};

	ReturnStatus rs{};
	std::chrono::steady_clock::time_point start{}, stop{};
	try {
		start = std::chrono::steady_clock::now();
		rs = impl->createReferenceDatabase(referenceTemplates,
		    args.dbDir, args.maximum);
		stop = std::chrono::steady_clock::now();
	} catch (const std::exception &e) {
		throw std::runtime_error("Exception while creating reference "
		    "database (" + std::string(e.what()) + ")");
	} catch (...) {
		throw std::runtime_error("Unknown exception while creating "
		    "reference database");
	}

	const std::string logName{"createReferenceDatabase.log"};
	std::ofstream file{args.outputDir / logName};
	if (!file)
		throw std::runtime_error(ts(getpid()) + ": Error creating log "
		    "file");

	static const std::string header{"elapsed,result,\"message\",max_size"};
	file << header << '\n';
	if (!file)
		throw std::runtime_error("Error writing to log");

	file << duration(start, stop) << ',' << e2i2s(rs.result) << ",\"" <<
	    (rs.message ? *rs.message : "") << "\"," << ts(args.maximum) <<
	    '\n';
	if (!file)
		throw std::runtime_error("Error writing to log");

	return (rs ? EXIT_SUCCESS : EXIT_FAILURE);
}

void
ELFT::Validation::runExtractionCreate(
    std::shared_ptr<ExtractionInterface> impl,
    const std::vector<uint64_t> &indicies,
    const Arguments &args)
{
	std::filesystem::create_directory(args.outputDir / Data::TemplateDir,
	    args.outputDir);
	for (const auto &dir : std::vector<std::filesystem::path>{
	    args.outputDir / Data::ProbeTemplateDir,
	    args.outputDir / Data::ReferenceTemplateDir})
		std::filesystem::create_directory(dir,
		    args.outputDir / Data::TemplateDir);

	const std::string logName{"extractionCreate-" +
	    e2i2s(*args.templateType) + '-' + ts(getpid()) + ".log"};
	std::ofstream file{args.outputDir / logName};
	if (!file)
		throw std::runtime_error(ts(getpid()) + ": Error creating log "
		    "file");

	static const std::string header{"\"identifier\",elapsed,result,"
	    "\"message\",type,num_images,size"};
	file << header << '\n';
	if (!file)
		throw std::runtime_error(ts(getpid()) + ": Error writing to "
		    "log");

	for (const auto &n : indicies) {
		file << performSingleCreate(impl, n, args) << '\n';
		if (!file)
			throw std::runtime_error(ts(getpid()) + ": Error "
			    "writing to log");
	}
}

void
ELFT::Validation::runExtractionExtractData(
    std::shared_ptr<ExtractionInterface> impl,
    const std::vector<uint64_t> &indicies,
    const Arguments &args)

{
	static const std::string header{"\"template_filename\",elapsed,"
	    "type,index,num_templates_in_buffer,image_identifier,quality,"
	    "imp,frct,frgp,orientation,lpm,value_assessment,lsb,pat,plr,trv,"
	    "\"cores\",\"deltas\",\"minutia\",\"roi\""};

	const std::string logName{"extractionData-" +
	    e2i2s(*args.templateType) + '-' + ts(getpid()) + ".log"};
	std::ofstream file{args.outputDir / logName};
	if (!file)
		throw std::runtime_error(ts(getpid()) + ": Error creating log "
		    "file");

	file << header << '\n';
	if (!file)
		throw std::runtime_error(ts(getpid()) + ": Error writing to "
		    "log");

	for (const auto &n : indicies) {
		std::string id{};
		std::tie(id, std::ignore) = getImageSet(n, *args.templateType);

		const std::filesystem::path f{
		    args.outputDir / Data::getTemplateDir(*args.templateType) /
		    std::string(id + Data::TemplateSuffix)};
		file << performSingleExtractData(impl, *args.templateType, f) <<
		    '\n';
	}
}
void
ELFT::Validation::runSearch(
    std::shared_ptr<SearchInterface> impl,
    const std::vector<uint64_t> &indicies,
    const Arguments &args)
{
	/* Configure candidate list log */
	const std::string candidateLogName{"searchCandidates-" + ts(getpid()) +
	    ".log"};
	std::ofstream candidateLog{args.outputDir / candidateLogName};
	if (!candidateLog)
		throw std::runtime_error(ts(getpid()) + ": Error creating "
		    "candidate log file");

	static const std::string candidateLogHeader{"\"identifier\","
	    "max_candidates,elapsed,result,\"message\",decision,num_candidates,"
	    "rank,\"candidate_identifier\",candidate_frgp,"
	    "candidate_similarity"};
	candidateLog << candidateLogHeader << '\n';
	if (!candidateLog)
		throw std::runtime_error(ts(getpid()) + ": Error writing to "
		    "candidate log");

	/* Configure correspondence log */
	const std::string corrLogName{"correspondence-" + ts(getpid()) +
	    ".log"};
	std::ofstream corrLog{args.outputDir / corrLogName};
	if (!corrLog)
		throw std::runtime_error(ts(getpid()) + ": Error creating "
		    "correspondence log file");

	static const std::string corrLogHeader{"\"probe_identifier\","
	    "num_candidates,elapsed,rank,correspondence_index,\"ref_id\","
	    "ref_input_id,ref_x,ref_y,ref_theta,ref_type,probe_input_id,"
	    "probe_x,probe_y,probe_theta,probe_type"};
	corrLog << corrLogHeader << '\n';
	if (!corrLog)
		throw std::runtime_error(ts(getpid()) + ": Error writing to "
		    "correspondence log");

	for (const auto &n : indicies) {
		/* Load template */
		std::string probeIdentifier{};
		std::tie(probeIdentifier, std::ignore) = Data::Probes.at(n);
		const auto probeTemplate = readFile(args.outputDir /
		    Data::ProbeTemplateDir /
		    (probeIdentifier + Data::TemplateSuffix));

		const auto &[searchResult, candidateLogLine] =
		    performSingleSearch(impl, probeIdentifier, probeTemplate,
		    static_cast<uint16_t>(args.maximum));
		candidateLog << candidateLogLine << '\n';

		corrLog << performSingleSearchExtract(impl, probeIdentifier,
		    probeTemplate, searchResult) << '\n';

		if (!candidateLog)
			throw std::runtime_error(ts(getpid()) + ": "
			    "Error writing to candidate log");
	}
}

std::string
ELFT::Validation::performSingleExtractData(
    const std::shared_ptr<ExtractionInterface> impl,
    TemplateType templateType,
    const std::filesystem::path &p)
{
	const CreateTemplateResult ctr{{}, readFile(p)};
	std::optional<std::tuple<ReturnStatus, std::vector<TemplateData>>>
	    ret{};

	std::chrono::steady_clock::time_point start{}, stop{};
	try {
		start = std::chrono::steady_clock::now();
		ret = impl->extractTemplateData(templateType, ctr);
		stop = std::chrono::steady_clock::now();
	} catch (const std::exception &e) {
		throw std::runtime_error("Exception while extracting data from "
		    "template " + p.string() + " (" + e.what() + ")");
	} catch (...) {
		throw std::runtime_error("Unknown exception while extracting "
		    "data from template " + p.string());
	}

	const std::string logLinePrefix{'"' + p.filename().string() + "\"," +
	    duration(start, stop) + ',' + e2i2s(templateType) + ','};

	if (!ret.has_value() || !std::get<ReturnStatus>(*ret)) {
		static const uint8_t numElements{18};
		static const std::string NAFull = splice(
		    std::vector<std::string>(numElements, NA), ",");
		return (logLinePrefix + NAFull);
	}

	const auto &data = std::get<std::vector<TemplateData>>(ret.value());

	std::string logLine{};
	for (std::vector<TemplateData>::size_type i{}; i < data.size(); ++i) {
		const auto &td = data.at(i);

		logLine += logLinePrefix + ts(i) + ',' + ts(data.size()) +
		    ',' + ts(td.inputIdentifier) + ',';
		logLine += (td.imageQuality ? ts(*td.imageQuality) : NA) + ',';

		static const uint8_t efsElements{14};
		static const std::string NAEFS = splice(
		    std::vector<std::string>(efsElements, NA), ",");
		if (!td.efs) {
			logLine += NAEFS + '\n';
			continue;
		}

		const auto &efs = td.efs.value();
		logLine += e2i2s(efs.imp) + ',' + e2i2s(efs.frct) + ',' +
		    e2i2s(efs.frgp) + ',';
		logLine += (efs.orientation ? ts(*efs.orientation) : NA) +
		    ',';
		logLine += (efs.lpm ? splice(*efs.lpm) : NA) + ',';
		logLine += (efs.valueAssessment ?
		    e2i2s(*efs.valueAssessment) : NA) + ',';
		logLine += (efs.lsb ? e2i2s(*efs.lsb) : NA) + ',';
		logLine += (efs.pat ? e2i2s(*efs.pat) : NA) + ',';
		logLine += (efs.plr ? ts(*efs.plr) : NA) + ',';
		logLine += (efs.trv ? ts(*efs.trv) : NA) + ',';
		logLine += (efs.cores ? '"' + splice(*efs.cores) + '"' : NA) +
		    ',';
		logLine += (efs.deltas ? '"' + splice(*efs.deltas)  + '"': NA) +
		    ',';
		logLine += (efs.minutiae ?
		    '"' + splice(*efs.minutiae) + '"' : NA) + ',';
		logLine += (efs.roi ? '"' + splice(*efs.roi)  + '"': NA);

		logLine += '\n';
	}

	/* Remove last newline */
	logLine.pop_back();
	return (logLine);
}

std::string
ELFT::Validation::performSingleCreate(
    const std::shared_ptr<ExtractionInterface> impl,
    const uint64_t imageIndex,
    const Arguments &args)
{
	const auto &[identifier, mds] = getImageSet(imageIndex,
	    *args.templateType);
	std::vector<std::tuple<std::optional<Image>, std::optional<EFS>>>
	    samples{};
	for (decltype(mds)::size_type i{}; i < mds.size(); ++i) {
		const auto &md = mds.at(i);

		if (!md.filename && !md.efs)
			throw std::runtime_error("No filename or EFS data "
			    "provided for imageIndex = " + ts(imageIndex));

		if (md.filename) {
			if (!md.width || !md.height || !md.ppi || !md.bpc ||
			    !md.bpp)
				throw std::runtime_error("Missing image meta"
				    "data for imageIndex = " + ts(imageIndex));

			if (md.efs && (md.efs->identifier != i))
				throw std::runtime_error("ID != for Image and "
				    "EFS for imageIndex = " + ts(imageIndex));

			samples.emplace_back(Image(static_cast<uint8_t>(i),
			    *md.width, *md.height, *md.ppi, *md.bpc, *md.bpp,
			    readFile(args.imageDir / *md.filename)), md.efs);
			const uint64_t expectedSize{
			    static_cast<uint64_t>(*md.bpp / 8) *
			    (*md.width) * (*md.height)};
			if (std::get<std::optional<ELFT::Image>>(
			    samples.back()).value().pixels.size() !=
			    expectedSize)
				throw std::runtime_error{"Did not read image "
				    "correctly for imageIndex = " +
				    ts(imageIndex) + " (expected " +
				    std::to_string(expectedSize) + ", read " +
				    std::to_string(std::get<
				    std::optional<ELFT::Image>>(
				    samples.back()).value().pixels.size()) +
				    ')'};
		} else
			samples.emplace_back(std::nullopt, md.efs);
	}

	CreateTemplateResult rv{};
	std::chrono::steady_clock::time_point start{}, stop{};
	try {
		start = std::chrono::steady_clock::now();
		rv = impl->createTemplate(*args.templateType, identifier,
		    samples);
		stop = std::chrono::steady_clock::now();
	} catch (const std::exception &e) {
		throw std::runtime_error("Exception while creating template "
		    "from " + identifier + " (" + e.what() + ")");
	} catch (...) {
		throw std::runtime_error("Unknown exception while creating "
		    "template from " + identifier);
	}

	std::string logLine{'"' + identifier + "\"," + duration(start, stop) +
	    ',' + e2i2s(rv.status.result) + ',' + sanitizeMessage(
	    rv.status.message ? *rv.status.message : "") + ',' +
	    e2i2s(*args.templateType) + ',' + ts(samples.size()) + ','};

	/* Write template */
	const auto dir = args.outputDir /
	    Data::getTemplateDir(*args.templateType);
	if (rv.status.result == ReturnStatus::Result::Success) {
		writeFile(rv.data, dir / (identifier + Data::TemplateSuffix));
		logLine += ts(rv.data.size());
	} else {
		writeFile({}, dir / (identifier + Data::TemplateSuffix));
		logLine += NA;
	}

	return (logLine);
}

std::tuple<ELFT::SearchResult, std::string>
ELFT::Validation::performSingleSearch(
    const std::shared_ptr<SearchInterface> impl,
    const std::string &identifier,
    const std::vector<std::byte> &probeTemplate,
    const uint16_t maxCandidates)
{
	/*
	 * NOTE: We don't search 0-byte templates, even if that's what was
	 *       returned, but we also don't want your submission to crash.
	 *       Hopefully passing a 0-byte template during validation will
	 *       encourage you to validate templates first.
	 */
// 	if (probeTemplate.size() == 0) {
// 		static const uint8_t numElements{9};
// 		static const std::string NAFull = splice(
// 		    std::vector<std::string>(numElements, NA), ",");
// 		return ('"' + identifier + "\"," + ts(maxCandidates) + NAFull);
// 	}

	SearchResult rv{};
	std::chrono::steady_clock::time_point start{}, stop{};
	try {
		start = std::chrono::steady_clock::now();
		rv = impl->search(probeTemplate, maxCandidates);
		stop = std::chrono::steady_clock::now();
	} catch (const std::exception &e) {
		throw std::runtime_error("Exception while searching template "
		    "for " + identifier + " (" + e.what() + ")");
	} catch (...) {
		throw std::runtime_error("Unknown exception while searching "
		    "template for " + identifier);
	}

	const std::string logLinePrefix{'"' + identifier + "\"," +
	    ts(maxCandidates) + ',' + duration(start, stop) + ',' +
	    e2i2s(rv.status.result) + ',' +
	    sanitizeMessage(rv.status.message ? *rv.status.message : "") + ','};
	std::string logLine{};
	if (rv.status && (rv.candidateList.size() > 0)) {
		/* API says driver will stable sort by similarity */
		std::stable_sort(rv.candidateList.begin(),
		    rv.candidateList.end(), std::greater<Candidate>());

		std::vector<Candidate>::size_type rank{};
		for (const auto &c : rv.candidateList) {
			logLine += logLinePrefix + ts(rv.decision) + ',' +
			    ts(rv.candidateList.size()) + ',' + ts(++rank) +
			    ",\"" + c.identifier + "\"," + e2i2s(c.frgp) + ',' +
			    ts(c.similarity);
			if (rank < rv.candidateList.size())
				logLine += '\n';
		}
	} else {
		logLine += logLinePrefix + splice(
		    std::vector<std::string>(6, NA), ",");
	}

	return {rv, logLine};
}

std::string
ELFT::Validation::performSingleSearchExtract(
    const std::shared_ptr<SearchInterface> impl,
    const std::string &identifier,
    const std::vector<std::byte> &probeTemplate,
    const SearchResult &searchResult)
{
	/*
	 * NOTE: We don't search 0-byte templates, even if that's what was
	 *       returned, but we also don't want your submission to crash.
	 *       Hopefully passing a 0-byte template during validation will
	 *       encourage you to validate templates first.
	 */
// 	if (probeTemplate.size() == 0) {
// 		static const uint8_t numElements{16};
// 		static const std::string NAFull = splice(
// 		    std::vector<std::string>(numElements, NA), ",");
// 		return ('"' + identifier + "\"," + NAFull);
// 	}

	std::optional<std::tuple<ReturnStatus,
	    std::vector<std::vector<Correspondence>>>> ret{};
	std::chrono::steady_clock::time_point start{}, stop{};
	try {
		start = std::chrono::steady_clock::now();
		ret = impl->extractCorrespondence(probeTemplate,
		    searchResult);
		stop = std::chrono::steady_clock::now();
	} catch (const std::exception &e) {
		throw std::runtime_error("Exception while extracting "
		    "correspondence for " + identifier + " (" + e.what() + ")");
	} catch (...) {
		throw std::runtime_error("Unknown exception while extracting "
		    "correspondence for " + identifier);
	}

	const std::string logLinePrefix{'"' + identifier + "\"," +
	    ts(searchResult.candidateList.size()) + ',' +
	    duration(start, stop) + ','};

	if (!ret.has_value() || !std::get<ReturnStatus>(*ret)) {
		static const uint8_t numElements{13};
		static const std::string NAFull = splice(
		    std::vector<std::string>(numElements, NA), ",");
		return (logLinePrefix + NAFull);
	}

	const auto &corrs =
	    std::get<std::vector<std::vector<Correspondence>>>(*ret);
	if (searchResult.candidateList.size() != corrs.size())
		throw std::runtime_error{"Number of entries in returned vector "
		    "of Correspondences must be the same as the number of "
		    "Candidates."};

	std::string logLine{};
	std::vector<std::vector<Correspondence>>::size_type rank{};
	for (const auto &candidate : corrs) {
		++rank;
		std::vector<Correspondence>::size_type corrIndex{};
		for (const auto &corr : candidate) {
			logLine += logLinePrefix +
			    ts(rank) + ',' + ts(++corrIndex) + ',' +
			    '"' + corr.referenceIdentifier + "\"," +
			    ts(corr.referenceInputIdentifier) + ',' +
			    ts(corr.referenceMinutia.coordinate.x) + ',' +
			    ts(corr.referenceMinutia.coordinate.y) + ',' +
			    ts(corr.referenceMinutia.theta) + ',' +
			    e2i2s(corr.referenceMinutia.type) + ',' +
			    ts(corr.probeInputIdentifier) + ',' +
			    ts(corr.probeMinutia.coordinate.x) + ',' +
			    ts(corr.probeMinutia.coordinate.y) + ',' +
			    ts(corr.probeMinutia.theta) + ',' +
			    e2i2s(corr.probeMinutia.type) + '\n';
		}
	}

	/* Remove last newline */
	logLine.pop_back();

	return (logLine);
}

std::string
ELFT::Validation::sanitizeMessage(
    const std::string &message,
    const bool escapeQuotes,
    const bool wrapInQuotes)
{
	if (message.empty())
		return (wrapInQuotes ? "\"\"" : message);

	std::string sanitized{message};

	/* Replace characters not matching the documented regex with space */
	auto it = sanitized.begin();
	while ((it = std::find_if_not(sanitized.begin(), sanitized.end(),
	    [](const char &c) -> bool {
		return (std::isgraph(c) || c == ' ');
	    })) != sanitized.end()) {
		sanitized.replace(it, std::next(it), " ");
	}

	/* Replace " with \" (we log to quoted CSV columns) */
	if (escapeQuotes) {
		static const std::string from{"\""};
		static const std::string to{"\\\""};
		std::string::size_type position{0};
		while ((position = sanitized.find(from, position)) !=
		    std::string::npos) {
			sanitized.replace(position, from.length(), to);
			position += to.length();
		}
	}
	return (wrapInQuotes ? '"' + sanitized + '"' : sanitized);
}

std::vector<std::vector<uint64_t>>
ELFT::Validation::splitSet(
    const std::vector<uint64_t> &combinedSet,
    const uint8_t numSets)
{
	if (numSets == 0)
		return {};
	if (numSets == 1)
		return {combinedSet};

	const std::vector<uint64_t>::size_type size{static_cast<
	    std::vector<uint64_t>::size_type>(
	    std::ceil(static_cast<float>(combinedSet.size()) /
	    static_cast<float>(numSets)))};
	if (size < numSets)
		throw std::invalid_argument("Too many sets.");

	std::vector<std::vector<uint64_t>> sets{};
	sets.reserve(numSets);
	for (uint8_t i{0}; i < numSets; ++i)
		sets.emplace_back(std::next(combinedSet.begin(),
		    static_cast<std::vector<uint64_t>::difference_type>(
		    size * i)),
		    std::next(combinedSet.begin(),
		    static_cast<std::vector<uint64_t>::difference_type>(
		    std::min(size * (i + 1u), combinedSet.size()))));

	return (sets);
}

void
ELFT::Validation::testOperation(
    const Arguments &args)
{
	if ((args.operation != Operation::Extract) &&
	    (args.operation != Operation::Search))
		throw std::runtime_error("Unsupported operation was sent to "
		    "testOperation()");

	std::filesystem::create_directory(args.outputDir);
	std::filesystem::permissions(args.outputDir,
	    std::filesystem::perms::owner_all |
	    std::filesystem::perms::group_all);

	/* Randomize the indicies of the sample data vectors. */
	uint64_t containerSize{};
	switch (args.operation.value()) {
	case Operation::Extract:
		switch (args.templateType.value()) {
		case TemplateType::Probe:
			containerSize = Data::Probes.size();
			break;
		case TemplateType::Reference:
			containerSize = Data::References.size();
			break;
		}
		break;
	case Operation::Search:
		containerSize = Data::Probes.size();
		break;
	default:
		throw std::runtime_error("Unsupported operation was send to "
		    "testOperation()");
	}
	const auto indicies = randomizeIndicies(containerSize, args.randomSeed);

	/* Instantiate only the appropriate interface */
	std::variant<std::shared_ptr<ELFT::ExtractionInterface>,
	    std::shared_ptr<ELFT::SearchInterface>> impl{};
	switch (args.operation.value()) {
	case Operation::Extract:
		impl = ELFT::ExtractionInterface::getImplementation(
		    args.configDir);
		break;
	case Operation::Search:
		impl = ELFT::SearchInterface::getImplementation(
		    args.configDir, args.dbDir);
		break;
	default:
		throw std::runtime_error("Unsupported operation was sent to "
		    "testOperation()");
		break;
	}

	if (args.numProcs <= 1) {
		switch (args.operation.value()) {
		case Operation::Extract:
			runExtractionCreate(std::get<std::shared_ptr<
			    ELFT::ExtractionInterface>>(impl), indicies, args);
 			runExtractionExtractData(std::get<std::shared_ptr<
 			    ELFT::ExtractionInterface>>(impl), indicies, args);
			break;
		case Operation::Search:
 			runSearch(std::get<std::shared_ptr<
 			    ELFT::SearchInterface>>(impl), indicies, args);
			break;
		default:
			throw std::runtime_error("Unsupported operation was "
			    "sent to testOperation()");
			break;
		}
	} else {
		/* Split into multiple sets of indicies */
		const auto sets = splitSet(indicies, args.numProcs);

		/* Fork */
		for (const auto &set : sets) {
			const auto pid = fork();
			switch (pid) {
			case 0:		/* Child */
				try {
					switch (args.operation.value()) {
					case Operation::Extract:
						runExtractionCreate(std::get<
						    std::shared_ptr<
						    ELFT::ExtractionInterface>>(
						    impl), set, args);
		 				runExtractionExtractData(std::
		 				    get<std::shared_ptr<
		 				    ELFT::ExtractionInterface>>(
		 				    impl), set, args);
						break;
					case Operation::Search:
 						runSearch(std::get<
 						    std::shared_ptr<
 						    ELFT::SearchInterface>>(
 						    impl), set, args);
						break;
					default:
						throw std::runtime_error(
						    "Invalid operation sent to "
						    "testOperation()");
					}
				} catch (const std::exception &e) {
					std::cerr << e.what() << '\n';
					std::exit(EXIT_FAILURE);
				} catch (...) {
					std::cerr << "Caught unknown "
					    "exception\n";
					std::exit(EXIT_FAILURE);
				}
				std::exit(EXIT_SUCCESS);

				/* Not reached */
				break;
			case -1:	/* Error */
				throw std::runtime_error("Error during fork()");
			default:	/* Parent */
				break;
			}
		}

		waitForExit(args.numProcs);
	}

	if (args.operation.value() == Operation::Extract)
		makeReferenceTemplateArchive(args);
}

void
ELFT::Validation::waitForExit(
    const uint8_t numChildren)
{
	pid_t pid{-1};
	bool stop{false};
	uint8_t exitedChildren{0};
	int status{};
	while (exitedChildren != numChildren) {
		stop = false;
		while (!stop) {
			pid = ::wait(&status);
			switch (pid) {
			case 0:		/* Status not available */
				break;
			case -1:	/* Delivery of signal */
				switch (errno) {
				case ECHILD:	/* No child processes remain */
					stop = true;
					break;
				case EINTR:	/* Interruption, try again */
					break;
				default:
					throw std::runtime_error{"Error while "
					    "reaping: " + std::system_error(
					        errno, std::system_category()).
					        code().message()};
				}
				break;
			default:	/* Child exited */
				++exitedChildren;
				break;
			}
		}
	}
}

void
ELFT::Validation::writeFile(
    const std::vector<std::byte> &data,
    const std::string &pathName)
{
	std::ofstream file{pathName,
	    std::ifstream::binary | std::ifstream::trunc};
	if (!file)
		throw std::runtime_error{"Could not open " + pathName};

	if (!file.write(reinterpret_cast<const char*>(data.data()),
	    static_cast<std::streamsize>(data.size())))
		throw std::runtime_error("Could not write " +
		    ts(data.size()) + " bytes to " + pathName);
}

int
main(
    int argc,
    char *argv[])
{
	if (!((ELFT::API_MAJOR_VERSION == 1) &&
	    (ELFT::API_MINOR_VERSION == 1))) {
		std::cerr << "Incompatible API version encountered.\n "
		    "- Validation: 1.1.X\n - Participant: " <<
		    ELFT::API_MAJOR_VERSION << '.' <<
		    ELFT::API_MINOR_VERSION << '.' <<
		    ELFT::API_PATCH_VERSION << '\n';
		std::cerr << "Rebuild your core library with the latest "
		    "elft.h\n";
		return (EXIT_FAILURE);
	}

	ELFT::Validation::Arguments args{};
	try {
		args = ELFT::Validation::parseArguments(argc, argv);
	} catch (const std::exception &e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		std::cerr << ELFT::Validation::getUsageString(argv[0]) <<
		    '\n';
		return (EXIT_FAILURE);
	}

	try {
		return (ELFT::Validation::dispatchOperation(args));
	} catch (const std::exception &e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return (EXIT_FAILURE);
	} catch (...) {
		std::cerr << "[ERROR] Caught non-standard exception." << '\n';
		return (EXIT_FAILURE);
	}
}
