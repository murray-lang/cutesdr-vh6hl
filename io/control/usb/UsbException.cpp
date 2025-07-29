//
// Created by murray on 12/04/23.
//

#include "UsbException.h"
#include <codecvt>
#include <locale>

static std::wstring_convert<std::codecvt_utf8<wchar_t>> wide2narrow;

UsbException::UsbException(const std::string& what)
    : std::runtime_error(what) {
}

UsbException::UsbException(const std::wstring& what)
    : std::runtime_error(wide2narrow.to_bytes(what)) {
}
