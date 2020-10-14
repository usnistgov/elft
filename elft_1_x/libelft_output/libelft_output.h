/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef ELFT_OUTPUT_H_
#define ELFT_OUTPUT_H_

/*
 * Output stream and conversion to string operators for ELFT types.
 *
 * When submitting to ELFT, do NOT link against this library.
 */

#include <ostream>
#include <string>
#include <sstream>

#include <elft.h>

namespace ELFT
{
	std::ostream& operator<<(std::ostream&, const Impression&);
	std::ostream& operator<<(std::ostream&,
	    const FrictionRidgeCaptureTechnology&);
	std::ostream& operator<<(std::ostream&,
	    const FrictionRidgeGeneralizedPosition&);
	std::ostream& operator<<(std::ostream&, const ProcessingMethod&);
	std::ostream& operator<<(std::ostream&, const PatternClassification&);
	std::ostream& operator<<(std::ostream&, const ValueAssessment&);
	std::ostream& operator<<(std::ostream&, const Substrate&);
	std::ostream& operator<<(std::ostream&, const RidgeQuality&);
	std::ostream& operator<<(std::ostream&, const ReturnStatus&);
	std::ostream& operator<<(std::ostream&, const ReturnStatus::Result&);
	std::ostream& operator<<(std::ostream&, const Coordinate&);
	std::ostream& operator<<(std::ostream&, const MinutiaType&);
	std::ostream& operator<<(std::ostream&, const Minutia&);
	std::ostream& operator<<(std::ostream&, const Correspondence&);
	std::ostream& operator<<(std::ostream&, const TemplateData&);
	std::ostream& operator<<(std::ostream&, const Candidate&);
	std::ostream& operator<<(std::ostream&, const SearchResult&);
	std::ostream& operator<<(std::ostream&,
	    const ProductIdentifier::CBEFFIdentifier&);
	std::ostream& operator<<(std::ostream&, const ProductIdentifier&);
	std::ostream& operator<<(std::ostream&,
	    const ExtractionInterface::SubmissionIdentification&);
	std::ostream& operator<<(std::ostream&, const CreateTemplateResult&);
	std::ostream& operator<<(std::ostream&, const Image&);
	std::ostream& operator<<(std::ostream&, const RidgeQualityRegion&);
	std::ostream& operator<<(std::ostream&, const EFS&);
	template<typename T,
	    std::enable_if_t<std::is_arithmetic<T>{}, int> = 0>
	    std::ostream& operator<<(std::ostream&, const std::optional<T>&);
	template<typename T,
	    std::enable_if_t<!std::is_arithmetic<T>{}, int> = 0>
	    std::ostream& operator<<(std::ostream&, const std::optional<T>&);

	std::string to_string(const Impression&);
	std::string to_string(const FrictionRidgeCaptureTechnology&);
	std::string to_string(const FrictionRidgeGeneralizedPosition&);
	std::string to_string(const ProcessingMethod&);
	std::string to_string(const PatternClassification&);
	std::string to_string(const ValueAssessment&);
	std::string to_string(const Substrate&);
	std::string to_string(const RidgeQuality&);
	std::string to_string(const ReturnStatus&);
	std::string to_string(const ReturnStatus::Result&);
	std::string to_string(const Coordinate&);
	std::string to_string(const MinutiaType&);
	std::string to_string(const Minutia&);
	std::string to_string(const Correspondence&);
	std::string to_string(const TemplateData&);
	std::string to_string(const Candidate&);
	std::string to_string(const SearchResult&);
	std::string to_string(const ProductIdentifier::CBEFFIdentifier&);
	std::string to_string(const ProductIdentifier&);
	std::string to_string(
	    const ExtractionInterface::SubmissionIdentification&);
	std::string to_string(const CreateTemplateResult&);
	std::string to_string(const Image&);
	std::string to_string(const RidgeQualityRegion&);
	std::string to_string(const EFS&);
	template<typename T,
	    std::enable_if_t<std::is_arithmetic<T>{}, int> = 0>
	    std::string to_string(const std::optional<T>&);
	template<typename T,
	    std::enable_if_t<!std::is_arithmetic<T>{}, int> = 0>
	    std::string to_string(const std::optional<T>&);
}

template<typename T,
    std::enable_if_t<std::is_arithmetic<T>{}, int>>
std::string
ELFT::to_string(
    const std::optional<T> &optionalT)
{
	if (!optionalT.has_value())
		return ("<# NOT SET #>");

	std::stringstream ss{};
	ss << std::boolalpha << optionalT.value();
	return (ss.str());
}

template<typename T,
    std::enable_if_t<!std::is_arithmetic<T>{}, int>>
std::string
ELFT::to_string(
    const std::optional<T> &optionalT)
{
	if (!optionalT.has_value())
		return ("<# NOT SET #>");

	return (to_string(optionalT.value()));
}

template<typename T,
    std::enable_if_t<std::is_arithmetic<T>{}, int>>
std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const std::optional<T> &optionalT)
{
	return (s << to_string(optionalT));
}

template<typename T,
    std::enable_if_t<!std::is_arithmetic<T>{}, int>>
std::ostream&
ELFT::operator<<(
    std::ostream &s,
    const std::optional<T> &optionalT)
{
	return (s << to_string(optionalT));
}

#endif /* ELFT_OUTPUT_H_ */
