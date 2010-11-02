/**
 *  Time-stamp:  <2010-10-28 14:00:26 raskolnikov>
 *
 *  @file        stereo.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 13:52:38 2010
 *
 *  Stereo sound.
 */

#ifndef PSYNTH_SOUND_STEREO_HPP_
#define PSYNTH_SOUND_STEREO_HPP_

/*
 *  Copyright (C) 2010 Juan Pedro Bolivar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cstddef>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/planar_frame_iterator.hpp>

namespace psynth
{
namespace sound
{

/**
   \addtogroup ChannelNameModel
   \{

   \brief Left
*/
struct left_channel {};    

/** \brief Right */
struct right_channel {};
 
/** \} */

/** \ingroup ChannelSpaceModel */
typedef mpl::vector3 <left_channel, right_channel> stereo_space;

/** \ingroup LayoutModel */
typedef layout<stereo_space> stereo_layout;

/** \ingroup LayoutModel */
typedef layout<stereo_space, mpl::vector3_c<int, 1, 0> > rl_stereo_layout;


/**
   \ingroup ImageViewConstructors
   \brief from raw stereo planar data
*/
template <typename IC>
inline
typename type_from_x_iterator<planar_frame_iterator<IC, stereo_space> >::view_t
planar_stereo_view (std::size_t size,
		    IC l, IC r,
		    std::ptrdiff_t rowsize_in_bytes)
{
    typedef typename type_from_x_iterator<
	planar_pixel_iterator<IC,rgb_t> >::view_t RView;
    return RView (size, planar_frame_iterator<IC, stereo_space> (l, r));
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_STEREO */