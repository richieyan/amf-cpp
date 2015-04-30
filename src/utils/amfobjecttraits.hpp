#pragma once
#ifndef AMFOBJECTTRAITS_HPP
#define AMFOBJECTTRAITS_HPP

#include <string>
#include <set>

namespace amf {

class AmfObjectTraits {
public:
	AmfObjectTraits(std::string className, bool dynamic, bool externalizable) :
		className(className), dynamic(dynamic), externalizable(externalizable) { }

	bool operator==(const AmfObjectTraits& other) const {
		return dynamic == other.dynamic &&
			externalizable == other.externalizable &&
			className == other.className &&
			attributes == other.attributes;
	}

	bool operator!=(const AmfObjectTraits& other) const {
		return !(*this == other);
	}

	std::string className;
	std::set<std::string> attributes;
	bool dynamic;
	bool externalizable;
};

} // namespace amf

#endif
