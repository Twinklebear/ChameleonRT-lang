#pragma once

#include <string>
#include <glm/glm.hpp>
#include "crtl/crtl_enums.h"
#include "crtl_export.h"

namespace crtl {

// Format the count as #G, #M, #K, depending on its magnitude
CRTL_EXPORT std::string pretty_print_count(const double count);

CRTL_EXPORT uint64_t align_to(uint64_t val, uint64_t align);

CRTL_EXPORT void ortho_basis(glm::vec3 &v_x, glm::vec3 &v_y, const glm::vec3 &n);

CRTL_EXPORT void canonicalize_path(std::string &path);

CRTL_EXPORT std::string get_file_extension(const std::string &fname);

CRTL_EXPORT std::string get_cpu_brand();

CRTL_EXPORT float srgb_to_linear(const float x);

CRTL_EXPORT float linear_to_srgb(const float x);

CRTL_EXPORT float luminance(const glm::vec3 &c);

CRTL_EXPORT std::wstring utf8_to_utf16(const std::string &utf8);

CRTL_EXPORT std::string utf16_to_utf8(const std::wstring &utf16);

CRTL_EXPORT size_t data_type_size(CRTL_DATA_TYPE type);
}
