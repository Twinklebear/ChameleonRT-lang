#pragma once

#include <string>
#include <glm/glm.hpp>
#include "crtl_export.h"

namespace crtl {

// Format the count as #G, #M, #K, depending on its magnitude
std::string pretty_print_count(const double count);

CRTL_EXPORT uint64_t align_to(uint64_t val, uint64_t align);

void ortho_basis(glm::vec3 &v_x, glm::vec3 &v_y, const glm::vec3 &n);

void canonicalize_path(std::string &path);

std::string get_file_extension(const std::string &fname);

std::string get_cpu_brand();

float srgb_to_linear(const float x);

float linear_to_srgb(const float x);

float luminance(const glm::vec3 &c);

std::wstring utf8_to_utf16(const std::string &utf8);

std::string utf16_to_utf8(const std::wstring &utf16);
}
