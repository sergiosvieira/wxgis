// 
// Boost.Process 
// ~~~~~~~~~~~~~ 
// 
// Copyright (c) 2006, 2007 Julio M. Merino Vidal 
// Copyright (c) 2008, 2009 Boris Schaeling 
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

/** 
 * \file boost/process.hpp 
 * 
 * Convenience header that includes all other Boost.Process public header 
 * files. It is important to note that those headers that are specific to 
 * a given platform are only included if the library is being used in that 
 * same platform. 
 */ 

#ifndef BOOST_PROCESS_HPP 
#define BOOST_PROCESS_HPP 

#include <wxgis/core/process/config.hpp> 

#if defined(BOOST_POSIX_API) 
#  include <wxgis/core/process/posix_child.hpp> 
#  include <wxgis/core/process/posix_context.hpp> 
#  include <wxgis/core/process/posix_operations.hpp> 
#  include <wxgis/core/process/posix_status.hpp> 
#elif defined(BOOST_WINDOWS_API) 
#  include <wxgis/core/process/win32_child.hpp> 
#  include <wxgis/core/process/win32_context.hpp> 
#  include <wxgis/core/process/win32_operations.hpp> 
#else 
#  error "Unsupported platform." 
#endif 

#include <wxgis/core/process/child.hpp> 
#include <wxgis/core/process/context.hpp> 
#include <wxgis/core/process/environment.hpp> 
#include <wxgis/core/process/operations.hpp> 
#include <wxgis/core/process/pistream.hpp> 
#include <wxgis/core/process/postream.hpp> 
#include <wxgis/core/process/process.hpp> 
#include <wxgis/core/process/self.hpp> 
#include <wxgis/core/process/status.hpp> 
#include <wxgis/core/process/stream_behavior.hpp> 

#endif 
