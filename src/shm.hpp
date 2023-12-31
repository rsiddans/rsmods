/***************************************************************************
                          shm.hpp  -  Shared memory mapping
                             -------------------
    begin                : Dec 24 2023
    copyright            : (C) 2023 by Gilles Duvert
    email                : surname dot name at free dot fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef SHM_HPP_
#define SHM_HPP_

#include "datatypes.hpp"
#include "envt.hpp"
#include <iostream>     // std::cout, std::ostream, std::ios
#include "shm_utils.hpp"
#ifndef _MSC_VER
extern std::map<DString, std::pair<DPtrGDL*, int> > shmList;

typedef std::map<DString, std::pair<DPtrGDL*, int> > ::iterator shmListIter;
namespace lib {

  void shmmap_pro(EnvT* e);
  void shmunmap_pro(EnvT* e);
  BaseGDL* shmvar_fun(EnvT* e);
  BaseGDL* shmdebug_fun(EnvT* e);

} // namespace

#endif
#endif
