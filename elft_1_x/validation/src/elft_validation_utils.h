/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef ELFT_VALIDATION_UTILS_H_
#define ELFT_VALIDATION_UTILS_H_

#include <algorithm>
#include <cctype>

namespace ELFT
{
	namespace Validation
	{
		const std::string NA{"NA"};

		/**
		 * @brief
		 * Cast an enumeration to its underlying integral value.
		 *
		 * @param t
		 * Item of enumeration type.
		 *
		 * @return
		 * Integral representation of t.
		 */
		template<typename T,
		    typename = typename std::enable_if<std::is_enum<T>::value>>
		typename std::underlying_type<T>::type
		e2i(
		    const T t)
		{
			return (static_cast<
			    typename std::underlying_type<T>::type>(t));
		}

		/**
		 * @brief
		 * Abbreviation for for std::to_string for integral types.
		 *
		 * @param t
		 * Item of integral type.
		 *
		 * @return
		 * String representation of t.
		 */
		template<typename T,
		    typename = typename
		    std::enable_if<std::is_integral<T>::value>>
		typename std::string
		ts(
		    const T t)
		{
			return (std::to_string(t));
		}

		/**
		 * @brief
		 * Obtain string value of an enumeration's underlying integral
		 * value.
		 *
		 * @param t
		 * Item of enumeration type.
		 *
		 * @return
		 * String representation of integral value of t.
		 */
		template<typename T,
		    typename = typename std::enable_if<std::is_enum<T>::value>>
		typename std::string
		e2i2s(
		    const T t)
		{
			return (ts(e2i(t)));
		}

		/**
		 * @brief
		 * Make a string lowercase in place.
		 *
		 * @param s
		 * The string to lowercase.
		 *
		 * @return
		 * Reference to `s` that has been modified to be lowercase.
		 */
		std::string&
		lower(
		    std::string &s)
		{
			std::transform(s.begin(), s.end(), s.begin(),
			    [](unsigned char c) {
				return (std::tolower(c));
			    });
			return (s);
		}

		/**
		 * @brief
		 * Make a log-able string out of a vector of Coordinate.
		 *
		 * @param v
		 * Vector of Coordinate to output.
		 * @param sep
		 * Token to separate multiple Coordinate.
		 *
		 * @return
		 * Log-able string version of `v`.
		 */
		std::string
		splice(
		    const std::vector<Coordinate> &v,
		    const std::string &sep = "|")
		{
			std::string ret{};
			for (const auto &c : v)
				ret += ts(c.x) + ';' + ts(c.y) + sep;
			ret.erase(ret.find_last_of(sep), sep.length());
			return (ret);
		}

		/**
		 * @brief
		 * Make a log-able string out of a vector of Minutia.
		 *
		 * @param v
		 * Vector of Minutia to output.
		 * @param sep
		 * Token to separate multiple Minutia.
		 *
		 * @return
		 * Log-able string version of `v`.
		 */
		std::string
		splice(
		    const std::vector<Minutia> &v,
		    const std::string &sep = "|")
		{
			std::string ret{};
			for (const auto &m : v)
				ret += ts(m.coordinate.x) + ';' +
				    ts(m.coordinate.y) + ';' + ts(m.theta) +
				    ';' + e2i2s(m.type) + sep;
			ret.erase(ret.find_last_of(sep), sep.length());
			return (ret);
		}

		/**
		 * @brief
		 * Make a log-able string out of a vector of Core.
		 *
		 * @param v
		 * Vector of Core to output.
		 * @param sep
		 * Token to separate multiple Core.
		 *
		 * @return
		 * Log-able string version of `v`.
		 */
		std::string
		splice(
		    const std::vector<Core> &v,
		    const std::string &sep = "|")
		{
			std::string ret{};
			for (const auto &c : v)
				ret += ts(c.coordinate.x) + ';' +
				    ts(c.coordinate.y) + ';' +
				    (c.direction ? ts(*c.direction) : NA) +
				    sep;
			ret.erase(ret.find_last_of(sep), sep.length());
			return (ret);
		}

		/**
		 * @brief
		 * Make a log-able string out of a vector of Delta.
		 *
		 * @param v
		 * Vector of Delta to output.
		 * @param sep
		 * Token to separate multiple Delta.
		 *
		 * @return
		 * Log-able string version of `v`.
		 */
		std::string
		splice(
		    const std::vector<Delta> &v,
		    const std::string &sep = "|")
		{
			std::string ret{};
			for (const auto &d : v) {
				ret += ts(d.coordinate.x) + ';' +
				    ts(d.coordinate.y) + ';';
				if (d.direction)
					ret +=
					    (std::get<0>(*d.direction) ?
					        ts(*std::get<0>(*d.direction)) :
					        NA) + ';' +
					    (std::get<1>(*d.direction) ?
					        ts(*std::get<1>(*d.direction)) :
					        NA) + ';' +
					    (std::get<2>(*d.direction) ?
					        ts(*std::get<2>(*d.direction)) :
					        NA) + ';';
				ret += sep;
			}
			ret.erase(ret.find_last_of(sep), sep.length());
			return (ret);
		}

		/**
		 * @brief
		 * Make a log-able string out of a vector of string.
		 *
		 * @param v
		 * Vector of string to output.
		 * @param sep
		 * Token to separate multiple strings.
		 *
		 * @return
		 * Log-able string version of `v`.
		 */
		std::string
		splice(
		    const std::vector<std::string> &v,
		    const std::string &sep)
		{
			std::string ret{};
			for (const auto &s : v)
				ret += s + sep;
			ret.erase(ret.find_last_of(sep), sep.length());
			return (ret);
		}

		/**
		 * @brief
		 * Make a log-able string out of a vector of enumerations.
		 *
		 * @param v
		 * Vector of enumerations to output.
		 * @param sep
		 * Token to separate multiple enumerations
		 *
		 * @return
		 * Log-able string version of `v`.
		 */
		template<typename T,
		    typename = typename std::enable_if<std::is_enum<T>::value>>
		std::string
		splice(
		    const std::vector<T> &v,
		    const std::string &sep = "|")
		{
			std::string ret{};
			for (const auto &e : v)
				ret += e2i2s(e) + sep;
			ret.erase(ret.find_last_of(sep), sep.length());
			return (ret);
		}

		/**
		 * @brief
		 * Make a log-able string of the difference of two times.
		 *
		 * @param start
		 * Start time.
		 * @param stop
		 * Stop time.
		 *
		 * @return
		 * String version of end - start, in microseconds.
		 */
		std::string
		duration(
		    const std::chrono::steady_clock::time_point &start,
		    const std::chrono::steady_clock::time_point &stop)
		{
			return (ts(std::chrono::duration_cast<
			    std::chrono::microseconds>(stop - start).count()));
		}
	}
}

#endif /* ELFT_VALIDATION_UTILS_H_ */
