/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "libelft_output.h"

std::string
ELFT::to_string(
    const Impression &imp)
{
	switch (imp) {
	case Impression::PlainContact:
		return ("PlainContact");
	case Impression::RolledContact:
		return ("RolledContact");
	case Impression::Latent:
		return ("Latent");
	case Impression::LiveScanSwipe:
		return ("LiveScanSwipe");
	case Impression::PlainContactlessStationary:
		return ("PlainContactlessStationary");
	case Impression::RolledContactlessStationary:
		return ("RolledContactlessStationary");
	case Impression::Other:
		return ("Other");
	case Impression::Unknown:
		return ("Unknown");
	case Impression::RolledContactlessMoving:
		return ("RolledContactlessMoving");
	case Impression::PlainContactlessMoving:
		return ("PlainContactlessMoving");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const FrictionRidgeCaptureTechnology &frct)
{
	switch (frct) {
	case FrictionRidgeCaptureTechnology::Unknown:
		return ("Unknown");
	case FrictionRidgeCaptureTechnology::ScannedInkOnPaper:
		return ("ScannedInkOnPaper");
	case FrictionRidgeCaptureTechnology::OpticalTIRBright:
		return ("OpticalTIRBright");
	case FrictionRidgeCaptureTechnology::OpticalDirect:
		return ("OpticalDirect");
	case FrictionRidgeCaptureTechnology::Capacitive:
		return ("Capacitive");
	case FrictionRidgeCaptureTechnology::Electroluminescent:
		return ("Electroluminescent");
	case FrictionRidgeCaptureTechnology::LatentImpression:
		return ("LatentImpression");
	case FrictionRidgeCaptureTechnology::LatentLift:
		return ("LatentLift");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const FrictionRidgeGeneralizedPosition &frgp)
{
 	switch (frgp) {
	case FrictionRidgeGeneralizedPosition::UnknownFinger:
		return ("UnknownFinger");
	case FrictionRidgeGeneralizedPosition::RightThumb:
		return ("RightThumb");
	case FrictionRidgeGeneralizedPosition::RightIndex:
		return ("RightIndex");
	case FrictionRidgeGeneralizedPosition::RightMiddle:
		return ("RightMiddle");
	case FrictionRidgeGeneralizedPosition::RightRing:
		return ("RightRing");
	case FrictionRidgeGeneralizedPosition::RightLittle:
		return ("RightLittle");
	case FrictionRidgeGeneralizedPosition::LeftThumb:
		return ("LeftThumb");
	case FrictionRidgeGeneralizedPosition::LeftIndex:
		return ("LeftIndex");
	case FrictionRidgeGeneralizedPosition::LeftMiddle:
		return ("LeftMiddle");
	case FrictionRidgeGeneralizedPosition::LeftRing:
		return ("LeftRing");
	case FrictionRidgeGeneralizedPosition::LeftLittle:
		return ("LeftLittle");
	case FrictionRidgeGeneralizedPosition::RightExtraDigit:
		return ("RightExtraDigit");
	case FrictionRidgeGeneralizedPosition::LeftExtraDigit:
		return ("LeftExtraDigit");
	case FrictionRidgeGeneralizedPosition::RightFour:
		return ("RightFour");
	case FrictionRidgeGeneralizedPosition::LeftFour:
		return ("LeftFour");
	case FrictionRidgeGeneralizedPosition::RightAndLeftThumbs:
		return ("RightAndLeftThumbs");
	case FrictionRidgeGeneralizedPosition::UnknownPalm:
		return ("UnknownPalm");
	case FrictionRidgeGeneralizedPosition::RightFullPalm:
		return ("RightFullPalm");
	case FrictionRidgeGeneralizedPosition::RightWritersPalm:
		return ("RightWritersPalm");
	case FrictionRidgeGeneralizedPosition::LeftFullPalm:
		return ("LeftFullPalm");
	case FrictionRidgeGeneralizedPosition::LeftWritersPalm:
		return ("LeftWritersPalm");
	case FrictionRidgeGeneralizedPosition::RightLowerPalm:
		return ("RightLowerPalm");
	case FrictionRidgeGeneralizedPosition::RightUpperPalm:
		return ("RightUpperPalm");
	case FrictionRidgeGeneralizedPosition::LeftLowerPalm:
		return ("LeftLowerPalm");
	case FrictionRidgeGeneralizedPosition::LeftUpperPalm:
		return ("LeftUpperPalm");
	case FrictionRidgeGeneralizedPosition::RightPalmOther:
		return ("RightPalmOther");
	case FrictionRidgeGeneralizedPosition::LeftPalmOther:
		return ("LeftPalmOther");
	case FrictionRidgeGeneralizedPosition::RightInterdigital:
		return ("RightInterdigital");
	case FrictionRidgeGeneralizedPosition::RightThenar:
		return ("RightThenar");
	case FrictionRidgeGeneralizedPosition::RightHypothenar:
		return ("RightHypothenar");
	case FrictionRidgeGeneralizedPosition::LeftInterdigital:
		return ("LeftInterdigital");
	case FrictionRidgeGeneralizedPosition::LeftThenar:
		return ("LeftThenar");
	case FrictionRidgeGeneralizedPosition::LeftHypothenar:
		return ("LeftHypothenar");
	case FrictionRidgeGeneralizedPosition::RightGrasp:
		return ("RightGrasp");
	case FrictionRidgeGeneralizedPosition::LeftGrasp:
		return ("LeftGrasp");
	case FrictionRidgeGeneralizedPosition::RightCarpalDeltaArea:
		return ("RightCarpalDeltaArea");
	case FrictionRidgeGeneralizedPosition::LeftCarpalDeltaArea:
		return ("LeftCarpalDeltaArea");
	case FrictionRidgeGeneralizedPosition::RightFullPalmAndWritersPalm:
		return ("RightFullPalmAndWritersPalm");
	case FrictionRidgeGeneralizedPosition::LeftFullPalmAndWritersPalm:
		return ("LeftFullPalmAndWritersPalm");
	case FrictionRidgeGeneralizedPosition::RightWristBracelet:
		return ("RightWristBracelet");
	case FrictionRidgeGeneralizedPosition::LeftWristBracelet:
		return ("LeftWristBracelet");
	case FrictionRidgeGeneralizedPosition::UnknownFrictionRidge:
		return ("UnknownFrictionRidge");
	case FrictionRidgeGeneralizedPosition::EJIOrTip:
		return ("EJIOrTip");
	default:
		return ("[ERROR]");
 	}
}

std::string
ELFT::to_string(
    const ProcessingMethod &pm)
{
	switch (pm) {
	case ProcessingMethod::Indanedione:
		return ("Indanedione");
	case ProcessingMethod::BlackPowder:
		return ("BlackPowder");
	case ProcessingMethod::Other:
		return ("Other");
	case ProcessingMethod::Cyanoacrylate:
		return ("Cyanoacrylate");
	case ProcessingMethod::Laser:
		return ("Laser");
	case ProcessingMethod::RUVIS:
		return ("RUVIS");
	case ProcessingMethod::StickysidePowder:
		return ("StickysidePowder");
	case ProcessingMethod::Visual:
		return ("Visual");
	case ProcessingMethod::WhitePowder:
		return ("WhitePowder");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const PatternClassification &pc)
{
	switch (pc) {
	case PatternClassification::Arch:
		return ("Arch");
	case PatternClassification::Whorl:
		return ("Whorl");
	case PatternClassification::RightLoop:
		return ("RightLoop");
	case PatternClassification::LeftLoop:
		return ("LeftLoop");
	case PatternClassification::Amputation:
		return ("Amputation");
	case PatternClassification::UnableToPrint:
		return ("UnableToPrint");
	case PatternClassification::Unclassifiable:
		return ("Unclassifiable");
	case PatternClassification::Scar:
		return ("Scar");
	case PatternClassification::DissociatedRidges:
		return ("DissociatedRidges");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const ValueAssessment &va)
{
	switch (va) {
	case ValueAssessment::Value:
		return ("Value");
	case ValueAssessment::Limited:
		return ("Limited");
	case ValueAssessment::NoValue:
		return ("NoValue");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const Substrate &sub)
{
	switch (sub) {
	case Substrate::Paper:
		return ("Paper");
	case Substrate::PorousOther:
		return ("PorousOther");
	case Substrate::Plastic:
		return ("Plastic");
	case Substrate::Glass:
		return ("Glass");
	case Substrate::MetalPainted:
		return ("MetalPainted");
	case Substrate::MetalUnpainted:
		return ("MetalUnpainted");
	case Substrate::TapeAdhesiveSide:
		return ("TapeAdhesiveSide");
	case Substrate::NonporousOther:
		return ("NonporousOther");
	case Substrate::PaperGlossy:
		return ("PaperGlossy");
	case Substrate::SemiporousOther:
		return ("SemiporousOther");
	case Substrate::Other:
		return ("Other");
	case Substrate::Unknown:
		return ("Unknown");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const RidgeQuality &rq)
{
	switch (rq) {
	case RidgeQuality::Background:
		return ("Background");
	case RidgeQuality::DebatableRidgeFlow:
		return ("DebatableRidgeFlow");
	case RidgeQuality::DebatableMinutiae:
		return ("DebatableMinutiae");
	case RidgeQuality::DefinitiveMinutiae:
		return ("DefinitiveMinutiae");
	case RidgeQuality::DefinitiveRidgeEdges:
		return ("DefinitiveRidgeEdges");
	case RidgeQuality::DefinitivePores:
		return ("DefinitivePores");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const ReturnStatus &rs)
{
	std::string s{"Result: " + to_string(rs.result)};
	if (rs.message)
		s += ", Message: " + *rs.message;
	return (s);
}

std::string
ELFT::to_string(
    const ReturnStatus::Result &r)
{
	switch (r) {
	case ReturnStatus::Result::Success:
		return ("Success");
	case ReturnStatus::Result::Failure:
		return ("Failure");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const Coordinate &c)
{
	return ('(' + std::to_string(c.x) + ',' + std::to_string(c.y) + ')');
}

std::string
ELFT::to_string(
    const MinutiaType &mt)
{
	switch (mt) {
	case MinutiaType::RidgeEnding:
		return ("RidgeEnding");
	case MinutiaType::Bifurcation:
		return ("Bifurcation");
	case MinutiaType::Other:
		return ("Other");
	case MinutiaType::Unknown:
		return ("Unknown");
	default:
		return ("[ERROR]");
	}
}

std::string
ELFT::to_string(
    const Minutia &m)
{
	return ('[' + to_string(m.coordinate) + ',' + std::to_string(m.theta) +
	    (m.theta > 359 ? " [INVALID: >359]" : "") + ',' +
	    to_string(m.type) + ']');
}

std::string
ELFT::to_string(
    const Core &c)
{
	return ('[' + to_string(c.coordinate) + ',' +
	    (c.direction ? std::to_string(*c.direction) : "<# EMPTY #>") +
	    (c.direction.value_or(0) > 359 ? " [INVALID: >359]" : "") + ']');
}

std::string
ELFT::to_string(
    const Delta &d)
{
	std::string s{'[' + to_string(d.coordinate) + ','};

	if (!d.direction)
		return (s + "<# EMPTY #>,<# EMPTY #>,<# EMPTY #>]");

	if (std::get<0>(*d.direction)) {
		s += std::to_string(*std::get<0>(*d.direction)) +
		    (*std::get<0>(*d.direction) > 359 ? " [INVALID: >359]" :
		    "") + ",";
	} else
		s+= "<# EMPTY #>,";

	if (std::get<1>(*d.direction)) {
		s += std::to_string(*std::get<1>(*d.direction)) +
		    (*std::get<1>(*d.direction) > 359 ? " [INVALID: >359]" :
		    "") + ",";
	} else
		s+= "<# EMPTY #>,";

	if (std::get<2>(*d.direction)) {
		s += std::to_string(*std::get<2>(*d.direction)) +
		    (*std::get<2>(*d.direction) > 359 ? " [INVALID: >359]" :
		    "");
	} else
		s+= "<# EMPTY #>";

	return (s + "]");
}

std::string
ELFT::to_string(
    const Correspondence &c)
{
	return ("Probe (ID #" + std::to_string(c.probeInputIdentifier) + ") " +
	    to_string(c.probeMinutia) + " == Reference (ID #" +
	    std::to_string(c.referenceInputIdentifier) + ") " +
	    to_string(c.referenceMinutia));
}

std::string
ELFT::to_string(
    const TemplateData &td)
{
	std::string s{"ID: " + td.candidateIdentifier + ", #" +
	    std::to_string(td.inputIdentifier)};
	if (td.efs) {
		s += "\n\tEFS:\n";
		std::stringstream ss{to_string(*td.efs)};
		for (std::string line{}; std::getline(ss, line); )
			s += "\t * " + line;
	}
	if (td.imageQuality) {
		s += "\n\tQuality: " + std::to_string(*td.imageQuality);
		if (*td.imageQuality > 100)
			s += " [INVALID: >100]";
	}

	return (s);
}

std::string
ELFT::to_string(
    const Candidate &c)
{
	return ("ID: " + c.identifier + ", FRGP: " + to_string(c.frgp) +
	    ", Similarity: " + std::to_string(c.similarity));
}

std::string
ELFT::to_string(
    const SearchResult &sr)
{
	std::string s{to_string(sr.status) + "\nCandidates:"};
	if (sr.candidateList.empty())
		s += " * [NO CANDIDATES]";
	else {
		for (const auto &c : sr.candidateList)
			s += " * " + to_string(c) + '\n';

		s += "Decision: ";
		if (sr.decision)
			s += "TRUE";
		else
			s += "FALSE";
	}

	return (s);
}

std::string
ELFT::to_string(
    const ProductIdentifier::CBEFFIdentifier &ci)
{
	std::stringstream ss{"Owner: 0x"};
	ss << std::hex << std::setw(4) << std::setfill('0') << ci.owner;
	if (ci.algorithm)
		ss << ", Algorithm: 0x" << std::hex << std::setw(4) <<
		    std::setfill('0') << *ci.algorithm;

	return (ss.str());
}

std::string
ELFT::to_string(
    const ProductIdentifier &pi)
{
	std::string s{};
	if (pi.marketing)
		s += "Marketing: " + *pi.marketing;
	if (pi.cbeff) {
		if (s.empty())
			s += ", ";
		s += to_string(*pi.cbeff);
	}

	return (s);
}

std::string
ELFT::to_string(
    const ExtractionInterface::SubmissionIdentification &si)
{
	std::stringstream ss{"Library Identifier: " + si.libraryIdentifier +
	    " (0x"};
	ss << std::hex << std::setw(4) << std::setfill('0') <<
	    si.versionNumber << '\n';
	if (si.exemplarAlgorithmIdentifier)
		ss << "Exemplar: " << *si.exemplarAlgorithmIdentifier;
	if (si.latentAlgorithmIdentifier) {
		if (si.exemplarAlgorithmIdentifier)
			ss << '\n';
		ss << "Latent: " << *si.latentAlgorithmIdentifier;
	}

	return (ss.str());
}

std::string
ELFT::to_string(
    const CreateTemplateResult &ct)
{
	std::string s{"ReturnStatus: " + to_string(ct.status)};
	if (ct.status.result == ReturnStatus::Result::Success)
		s += ", Template Size: " + std::to_string(ct.data.size()) + 'b';
	return (s);
}

std::string
ELFT::to_string(
    const Image &i)
{
	return ("ID #" + std::to_string(i.identifier) + ", Dimensions: " +
	    std::to_string(i.width) + 'x' + std::to_string(i.height) + ", "
	    "PPI: " + std::to_string(i.ppi) + ", BPC: " +
	    std::to_string(i.bpc) + ", BPP: " + std::to_string(i.bpp) + " "
	    "Size: " + std::to_string(i.pixels.size()) + 'b');
}

std::string
ELFT::to_string(
    const RidgeQualityRegion &rqr)
{
	std::string s{" * Region: "};
	if (rqr.region.empty()) {
		s += "<# EMPTY #>";
	} else {
		for (const auto &c : rqr.region)
			s += "\n   * " + to_string(c);
	}

	s += " * Quality: " + to_string(rqr.quality);

	return (s);
}

std::string
ELFT::to_string(
    const EFS &efs)
{
	std::string s{"ID #: " + std::to_string(efs.identifier) + '\n'};
	s += " * Impression: " + to_string(efs.imp) + '\n';
	s += " * FRCT: " + to_string(efs.frct) + '\n';
	s += " * FRGP: " + to_string(efs.frgp) + '\n';
	s += " * Orientation: " + to_string(efs.orientation) + '\n';

	s += " * Processing Method: ";
	if (efs.lpm) {
		if (efs.lpm->empty()) {
			s += "<# EMPTY #>";
		} else {
			for (const auto &l : *efs.lpm)
				s += "\n   * " + to_string(l);
		}
	} else {
		s += to_string(std::optional<ProcessingMethod>{});
	}
	s += '\n';

	s += " * Value Assessment: " + to_string(efs.valueAssessment) + '\n';
	s += " * Substrate: " + to_string(efs.lsb) + '\n';
	s += " * Pattern Classification: " + to_string(efs.pat) + '\n';
	s += " * Laterally Reversed: " + to_string(efs.plr) + '\n';
	s += " * Some/All Tonally Reversed: " + to_string(efs.trv) + '\n';

	s += " * Cores: ";
	if (efs.cores) {
		if (efs.cores->empty()) {
			s += "<# EMPTY #>";
		} else {
			for (const auto &c : *efs.cores)
				s += "\n   * " + to_string(c);
		}
	} else {
		s += to_string(std::optional<Coordinate>{});
	}
	s += '\n';

	s += " * Deltas: ";
	if (efs.deltas) {
		if (efs.deltas->empty()) {
			s += "<# EMPTY #>";
		} else {
			for (const auto &d : *efs.deltas)
				s += "\n   * " + to_string(d);
		}
	} else {
		s += to_string(std::optional<Coordinate>{});
	}
	s += '\n';

	s += " * Minutiae: ";
	if (efs.minutiae) {
		if (efs.minutiae->empty()) {
			s += "<# EMPTY #>";
		} else {
			for (const auto &m : *efs.minutiae)
				s += "\n   * " + to_string(m);
		}
	} else {
		s += to_string(std::optional<Minutia>{});
	}
	s += '\n';

	s += " * ROI: ";
	if (efs.roi) {
		if (efs.roi->empty()) {
			s += "<# EMPTY #>";
		} else {
			for (const auto &r : *efs.roi)
				s += "\n   * " + to_string(r);
		}
	} else {
		s += to_string(std::optional<Coordinate>{});
	}
	s += '\n';

	s += " * Ridge Quality Map: ";
	if (efs.rqm) {
		if (efs.rqm->empty()) {
			s += "<# EMPTY #>";
		} else {
			for (const auto &c : *efs.rqm)
				s += "\n   * " + to_string(c);
		}
	} else {
		s += to_string(std::optional<RidgeQualityRegion>{});
	}

	return (s);
}

/******************************************************************************/

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Impression &imp)
{
	return (s << ELFT::to_string(imp));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const FrictionRidgeCaptureTechnology &frct)
{
	return (s << ELFT::to_string(frct));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const FrictionRidgeGeneralizedPosition &frgp)
{
	return (s << ELFT::to_string(frgp));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const ProcessingMethod &pm)
{
	return (s << ELFT::to_string(pm));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const PatternClassification &pc)
{
	return (s << ELFT::to_string(pc));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const ValueAssessment &va)
{
	return (s << ELFT::to_string(va));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Substrate &sub)
{
	return (s << ELFT::to_string(sub));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const RidgeQuality &rq)
{
	return (s << ELFT::to_string(rq));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const ReturnStatus &rs)
{
	return (s << ELFT::to_string(rs));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const ReturnStatus::Result &r)
{
	return (s << ELFT::to_string(r));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Coordinate &c)
{
	return (s << ELFT::to_string(c));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const MinutiaType &mt)
{
	return (s << ELFT::to_string(mt));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Minutia &m)
{
	return (s << ELFT::to_string(m));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Core &c)
{
	return (s << ELFT::to_string(c));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Delta &d)
{
	return (s << ELFT::to_string(d));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Correspondence &c)
{
	return (s << ELFT::to_string(c));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const TemplateData &td)
{
	return (s << ELFT::to_string(td));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Candidate &c)
{
	return (s << ELFT::to_string(c));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const SearchResult &sr)
{
	return (s << ELFT::to_string(sr));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const ProductIdentifier::CBEFFIdentifier &ci)
{
	return (s << ELFT::to_string(ci));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const ProductIdentifier &pi)
{
	return (s << ELFT::to_string(pi));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const ExtractionInterface::SubmissionIdentification &si)
{
	return (s << ELFT::to_string(si));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const CreateTemplateResult &ct)
{
	return (s << ELFT::to_string(ct));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const Image &i)
{
	return (s << ELFT::to_string(i));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const RidgeQualityRegion &rqr)
{
	return (s << ELFT::to_string(rqr));
}

std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const EFS &efs)
{
	return (s << ELFT::to_string(efs));
}

