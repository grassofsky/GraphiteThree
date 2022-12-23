
/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000-2009 INRIA - Project ALICE
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: Bruno Levy - levy@loria.fr
 *
 *     Project ALICE
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 *
 * As an exception to the GPL, Graphite can be linked 
 *  with the following (non-GPL) libraries: Qt, SuperLU, WildMagic and CGAL
 */

#ifndef H_OGF_MESH_TOOLS_MESH_GROB_PAINT_TOOLS_H
#define H_OGF_MESH_TOOLS_MESH_GROB_PAINT_TOOLS_H

#include <OGF/mesh/common/common.h>
#include <OGF/mesh/tools/mesh_grob_tool.h>

/**
 * \file OGF/mesh/tools/mesh_grob_paint_tools.h
 * \brief Tools to paint attributes on meshes
 */

namespace OGF {

    
    gom_class MESH_API MeshGrobPaintTool : public MeshGrobTool {
    public:
        MeshGrobPaintTool(ToolsManager* parent);

        
    gom_properties:
         /**
          * \brief The value to be painted.
          */
         void set_value(double value) {
             value_ = value;
         }
         
         double get_value() const {
	    return value_;
	 }

         /**
          * \brief If set, value is added to
          *  previous value when painting.
          */
         void set_accumulate(bool value) {
	    accumulate_ = value;
	 }
         
         bool get_accumulate() const {
	    return accumulate_;
	 }

         /**
          * \brief If set, colormap range is
          *  continuously updated when painting.
          */
         void set_autorange(bool value) {
             autorange_ = value;
         }
         
         bool get_autorange() const {
             return autorange_;
         }
         
         /**
          * \brief If set, vertex attributes can be 
          *  only painted by directly picking vertices,
          *  else one can pick facets or cells.
          */
         void set_pick_vertices_only(bool value);
         
         bool get_pick_vertices_only() const {
             return pick_vertices_only_;
         }

    protected:
         void paint(const RayPick& p_ndc);
         
    protected:
         double value_;
         bool accumulate_;
         bool autorange_;
         bool pick_vertices_only_;
         
         double timestamp_;
         double update_time_;
         index_t picked_element_;
    };

    /**********************************************************/
    
    /**
     * \brief A tool that paints attribute values in a mesh
     */
    gom_attribute(category, "paint")
    gom_attribute(icon, "paint")
    gom_attribute(help, "paint attributes")
    gom_attribute(message, "btn1: paint; btn3: erase")
	   
    gom_class MESH_API MeshGrobPaint : public MeshGrobPaintTool {
    public:
        /**
         * \brief MeshGrobPaint constructor.
         * \param[in] parent a pointer to the ToolsManager
         */
        MeshGrobPaint(ToolsManager* parent);

        /**
         * \copydoc Tool::grab()
         */
        void grab(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::drag()
         */
        void drag(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::release()
         */
        void release(const RayPick& p_ndc) override;
        
    protected:
        vec2 latest_ndc_;
    };    

    /*************************************************************/

    /**
     * \brief A rect selection tool that paints attribute values in a mesh
     */
    gom_attribute(category, "paint")
    gom_attribute(icon, "paint_rect")
    gom_attribute(help, "paint attributes in rectangle")
    gom_attribute(message, "btn1: paint; btn3: erase")
	   
    gom_class MESH_API MeshGrobPaintRect : public MeshGrobPaintTool {
    public:
        /**
         * \brief MeshGrobPaint constructor.
         * \param[in] parent a pointer to the ToolsManager
         */
        MeshGrobPaintRect(ToolsManager* parent);

        /**
         * \copydoc Tool::grab()
         */
        void grab(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::drag()
         */
        void drag(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::release()
         */
        void release(const RayPick& p_ndc) override;

    gom_properties:

        /**
         * \brief If set, pick everything that falls
         *  within the rect, else pick only visible
         *  elements.
         */
        void set_xray_mode(bool x) {
            xray_mode_ = x;
        }

        bool get_xray_mode() const {
            return xray_mode_;
        }
        
    protected:

        /**
         * \brief Paints a rectangle
         * \param[in] raypick 
         * \param[in] x0 , y0 , x1 , y1 image bounds (device coordinates)
         * \param[in] mask an optional mask. Black pixels do not belong to
         *  the selection, non-zero pixels belong to selection. Image is
         *  supposed to be grayscale, 8 bits per pixels. Size is supposed to
         *  be (x1-x0+1) x (y1-y0+1)
         */
        void paint_rect(
            const RayPick& raypick,
            index_t x0, index_t y0, index_t x1, index_t y1,
            Image* mask = nullptr
        );
        
    private:
        bool xray_mode_;
        bool active_;
        vec2 p_;
    };    
    

    /*************************************************************/

    /**
     * \brief Paint attributes using free-form selection
     */
    gom_attribute(category, "paint")
    gom_attribute(icon, "paint_freeform")
    gom_attribute(help, "paint attributes with free-form selection")
    gom_attribute(message, "btn1: paint; btn3: erase")
	   
    gom_class MESH_API MeshGrobPaintFreeform : public MeshGrobPaintTool {
    public:
        /**
         * \brief MeshGrobPaintFreeform constructor.
         * \param[in] parent a pointer to the ToolsManager
         */
        MeshGrobPaintFreeform(ToolsManager* parent);
        
        /**
         * \copydoc Tool::grab()
         */
        void grab(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::drag()
         */
        void drag(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::release()
         */
        void release(const RayPick& p_ndc) override;

    gom_properties:

        /**
         * \brief If set, pick everything that falls
         *  within the rect, else pick only visible
         *  elements.
         */
        void set_xray_mode(bool x) {
            xray_mode_ = x;
        }

        bool get_xray_mode() const {
            return xray_mode_;
        }
    private:
        bool xray_mode_;
        bool active_;
        vec2 latest_ndc_;
        vector<vec2> selection_;
    };

    /*****************************************************************/

    /**
     * \brief Paint attributes in connected components
     */
    gom_attribute(category, "paint")
    gom_attribute(icon, "paint_connected")
    gom_attribute(help, "paint attributes on connected components")
    gom_attribute(message, "btn1: paint; btn3: erase")
	   
    gom_class MESH_API MeshGrobPaintConnected : public MeshGrobPaintTool {
    public:
        /**
         * \brief MeshGrobPaintConnected constructor.
         * \param[in] parent a pointer to the ToolsManager
         */
        MeshGrobPaintConnected(ToolsManager* parent);

        /**
         * \copydoc Tool::grab()
         */
        void grab(const RayPick& p_ndc) override;
    };

    /**********************************************************************/
    
    /**
     * \brief A tool that probes attribute values in a mesh
     */
    gom_attribute(category, "paint")
    gom_attribute(icon, "pipette")
    gom_attribute(help, "probe attributes")
    gom_attribute(message, "btn1: probe attributes")
    
    gom_class MESH_API MeshGrobProbe : public MeshGrobTool {
    public:
        /**
         * \brief MeshGrobPaint constructor.
         * \param[in] parent a pointer to the ToolsManager
         */
        MeshGrobProbe(ToolsManager* parent);

        /**
         * \copydoc Tool::grab()
         */
        void grab(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::drag()
         */
        void drag(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::release()
         */
        void release(const RayPick& p_ndc) override;

     protected:
        void probe(const RayPick& p_ndc);
        
    private:
        bool picked_;
        double value_;
        vec2 latest_ndc_;
    };    

    /********************************************************/
    
    /**
     * \brief A tool that measures distances.
     */
    gom_attribute(category, "paint")
    gom_attribute(icon, "ruler")
    gom_attribute(help, "measures distances on a mesh")
    
    gom_class MESH_API MeshGrobRuler : public MeshGrobTool {
    public:
        /**
         * \brief MeshGrobPaint constructor.
         * \param[in] parent a pointer to the ToolsManager
         */
        MeshGrobRuler(ToolsManager* parent);

        /**
         * \copydoc Tool::grab()
         */
        void grab(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::drag()
         */
        void drag(const RayPick& p_ndc) override;

        /**
         * \copydoc Tool::release()
         */
        void release(const RayPick& p_ndc) override;

     protected:
        bool pick(const RayPick& p_ndc, vec3& p);
        void show_distance(const RayPick& p_ndc);

    private:
        bool p_picked_;
        vec3 p_;
        vec2 latest_ndc_;
    };    

    /*******************************************************/
    
}

#endif

