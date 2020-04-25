#pragma once
#include <optional>

#define OPTIONAL_PROPERTY(TYPE, name) \
public:\
	inline const TYPE& get_##name() const\
	{\
		return name.value();\
	}\
	inline bool has_##name() const\
	{\
		return name.has_value();\
	}\
protected:\
	inline void set_##name(TYPE value)\
	{\
		name = value;\
	}\
	inline bool parse_##name(const std::string& inputString)\
	{\
		if (name.has_value()) return false;\
		set_##name(parse_delegate_##TYPE(inputString));\
		return name.has_value();\
	}\
private:\
	std::optional<TYPE> name;\
