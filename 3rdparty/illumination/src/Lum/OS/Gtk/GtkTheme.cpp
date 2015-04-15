/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <Lum/OS/Gtk/GtkTheme.h>

#include <Lum/Private/Config.h>

#include <gdk/gdk.h>

#if defined(LUM_HAVE_LIB_HILDON)
  #include <hildon/hildon-version.h>
  #include <hildon/hildon-window.h>

  #if HILDON_CHECK_VERSION(2,1,0)
    #include <hildon/hildon-check-button.h>
    #include <hildon/hildon-defines.h>
    #include <hildon/hildon-gtk.h>
    #include <hildon/hildon-entry.h>
    #include <hildon/hildon-picker-button.h>
  #endif
#endif

#include <Lum/Base/Size.h>

#if defined(LUM_HAVE_LIB_CAIRO)
  #include <Lum/OS/Cairo/DrawInfo.h>
#endif

#include <Lum/OS/Display.h>

#if GLIB_CHECK_VERSION(2,26,0)
  #include <gio/gio.h>
#endif

#if defined(HAVE_LIB_GCONF)
  #include <gconf/gconf-client.h>
#endif

#include <cmath>
#include <cstring>
#include <iostream>

#include <Lum/Base/String.h>

#include <Lum/Array.h>
#include <Lum/Button.h>
#include <Lum/Menu.h>
#include <Lum/Popup.h>

namespace Lum {
  namespace OS {
    namespace Gtk {

      GtkWidgets *gtkWidgets=NULL;

      static const char* buttonDetail="button";

      static GtkStateType GetGtkStateType(OS::DrawInfo *draw)
      {
        if (draw->disabled) {
          return GTK_STATE_INSENSITIVE;
        }
        else if (draw->selected) {
          return GTK_STATE_ACTIVE;
        }
        else if (draw->activated) {
          return GTK_STATE_PRELIGHT;
        }

        return GTK_STATE_NORMAL;
      }

      static GtkStateType GetGtkRangeStateType(OS::DrawInfo *draw, GtkWidget* widget)
      {
        if (GetWidgetStyleBoolValue(widget,"activate-slider")) {
          if (draw->disabled) {
            return GTK_STATE_INSENSITIVE;
          }
          else if (draw->selected) {
            return GTK_STATE_ACTIVE;
          }
          else if (draw->activated) {
            return GTK_STATE_PRELIGHT;
          }

          return GTK_STATE_NORMAL;
        }
        else {
          if (draw->disabled) {
            return GTK_STATE_INSENSITIVE;
          }
          else if (draw->selected || draw->activated) {
            return GTK_STATE_PRELIGHT;
          }

          return GTK_STATE_NORMAL;
        }
      }

      static void SetFocusIfRequested(OS::DrawInfo *draw, GtkWidgets::GtkStyles style)
      {
        if (draw->focused &&
            !draw->disabled) {
          GTK_WIDGET_SET_FLAGS(gtkWidgets->widget[style],GTK_HAS_FOCUS|GTK_HAS_DEFAULT);
        }
        else {
          GTK_WIDGET_UNSET_FLAGS(gtkWidgets->widget[style],GTK_HAS_FOCUS|GTK_HAS_DEFAULT);
        }
      }

      void GetButtonFrame(GtkWidget* button,
                          size_t& left, size_t& right, size_t& top, size_t& bottom)
      {
        GtkBorder *innerBorder=NULL,*defaultBorder=NULL,*defaultOutsideBorder=NULL;
        gboolean  interiorFocus;
        gint      focusWidth,focusPad;
        gint      borderWidth;

        gtk_widget_style_get(button,
                             "default-border",&defaultBorder,
                             "default-outside-border",&defaultOutsideBorder,
                             "inner-border",&innerBorder,
                             "interior-focus",&interiorFocus,
                             "focus-line-width", &focusWidth,
                             "focus-padding", &focusPad,
                             NULL);

        borderWidth=GTK_CONTAINER(button)->border_width;

        /* Calculate width of buttom frame */
        left=borderWidth;
        right=left;
        top=left;
        bottom=left;

        if (GTK_WIDGET_CAN_DEFAULT(button)) {
          if (defaultBorder!=NULL) {
            left+=defaultBorder->left;
            right+=defaultBorder->right;
            top+=defaultBorder->top;
            bottom+=defaultBorder->bottom;
          }
          else {
            left++;
            right++;
            top++;
            bottom++;
          }
        }

        if (innerBorder!=NULL) {
          left+=innerBorder->left;
          right+=innerBorder->right;
          top+=innerBorder->top;
          bottom+=innerBorder->bottom;
        }
        else {
          left++;
          right++;
          top++;
          bottom++;
        }

        left+=button->style->xthickness+focusPad+focusWidth;
        right+=button->style->xthickness+focusPad+focusWidth;
        top+=button->style->ythickness+focusPad+focusWidth;
        bottom+=button->style->ythickness+focusPad+focusWidth;

#if defined(LUM_HAVE_LIB_HILDON)
        left+=2;
        right+=2;
#if HILDON_CHECK_VERSION(2,1,0)
#else
        top+=4;
        bottom+=4;
#endif
#else
        left+=2;
        right+=2;
        top+=2;
        bottom+=2;
#endif
        if (defaultBorder!=NULL) {
          gtk_border_free(defaultBorder);
        }
        if (defaultOutsideBorder!=NULL) {
          gtk_border_free(defaultOutsideBorder);
        }
        if (innerBorder!=NULL) {
          gtk_border_free(innerBorder);
        }
      }

      static void DrawButton(GtkWidget* button,
                             OS::DrawInfo* draw,
                             ::GdkDrawable* gdkDraw,
                             ::GdkRectangle* rectangle,
                             int x, int y,
                             size_t width, size_t height)
      {
        GtkBorder   *innerBorder=NULL,*defaultBorder=NULL,*defaultOutsideBorder=NULL;
        gboolean    interiorFocus;
        int         focusWidth,focusPad;
        int         borderWidth;
        const char* detail="button";

        if (draw->focused) {
          GTK_WIDGET_SET_FLAGS(button,GTK_HAS_FOCUS|GTK_HAS_DEFAULT);
        }
        else {
          GTK_WIDGET_UNSET_FLAGS(button,GTK_HAS_FOCUS|GTK_HAS_DEFAULT);
        }

        gtk_widget_style_get(button,
                             "default-border",&defaultBorder,
                             "default-outside-border",&defaultOutsideBorder,
                             "inner-border",&innerBorder,
                             "interior-focus",&interiorFocus,
                             "focus-line-width", &focusWidth,
                             "focus-padding", &focusPad,
                             NULL);

        borderWidth=GTK_CONTAINER(button)->border_width;

        x+=borderWidth;
        y+=borderWidth;
        width-=2*borderWidth;
        height-=2*borderWidth;

        if (draw->focused &&
            GTK_WIDGET_CAN_DEFAULT(button) &&
            GTK_BUTTON(button)->relief==GTK_RELIEF_NORMAL) {
          gtk_paint_box(button->style,
                        gdkDraw,
                        GTK_STATE_NORMAL,
                        GTK_SHADOW_IN,
                        rectangle,
                        button,
                        "buttondefault",
                        x,y,width,height);

          if (defaultBorder!=NULL) {
            x+=defaultBorder->left;
            y+=defaultBorder->top;
            width-=defaultBorder->left+defaultBorder->right;
            height-=defaultBorder->top+defaultBorder->bottom;
          }
          else {
            x+=1;
            y+=1;
            width-=2;
            height-=2;
          }
        }
        else {
          if (defaultOutsideBorder!=NULL) {
            x+=defaultOutsideBorder->left;
            y+=defaultOutsideBorder->top;
            width-=defaultOutsideBorder->left+defaultOutsideBorder->right;
            height-=defaultOutsideBorder->top+defaultOutsideBorder->bottom;
          }
        }

        if (!interiorFocus && draw->focused) {
          x+=focusWidth+focusPad;
          y+=focusWidth+focusPad;
          width-=2*focusWidth+focusPad;
          height-=2*focusWidth+focusPad;
        }

        if (GTK_BUTTON(button)->relief!=GTK_RELIEF_NONE ||
            draw->selected ||
            draw->activated) {
          gtk_paint_box(button->style,
                        gdkDraw,
                        GetGtkStateType(draw),
                        draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                        rectangle,
                        button,
                        detail,
                        x,y,width,height);
        }

        if (draw->focused) {
          if (interiorFocus) {
            x+=button->style->xthickness+focusPad;
            y+=button->style->ythickness+focusPad;
            width-=2*(button->style->xthickness+focusPad);
            height-=2*(button->style->ythickness+focusPad);
          }
          else {
            x-=focusWidth+focusPad;
            y-=focusWidth+focusPad;
            width+=2*(focusWidth+focusPad);
            height+=2*(focusWidth+focusPad);
          }

          gtk_paint_focus(button->style,
                          gdkDraw,
                          GetGtkStateType(draw),
                          rectangle,
                          button,
                          detail,
                          x,y,width,height);
        }

        if (defaultBorder!=NULL) {
          gtk_border_free(defaultBorder);
        }
        if (defaultOutsideBorder!=NULL) {
          gtk_border_free(defaultOutsideBorder);
        }
        if (innerBorder!=NULL) {
          gtk_border_free(innerBorder);
        }
      }

      static bool UseScrolledBackgroundFill()
      {
        GtkShadowType shadowType=GTK_SHADOW_NONE;

        g_object_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                     "shadow-type",&shadowType,NULL);

        return shadowType!=GTK_SHADOW_NONE;
      }

      GtkWidgets::GtkWidgets()
      {
        GtkListStore      *listStore;
        GtkCellRenderer   *cellRenderer;
        GtkTreeIter       treeIter;
        GtkTreeViewColumn *column;
        GtkTreeView       *list2;
#if defined(LUM_HAVE_LIB_HILDON)
        widget[styleWindow]=hildon_window_new();
#else
        widget[styleWindow]=gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif
        gtk_widget_realize(widget[styleWindow]);

        layout=gtk_fixed_new();
        gtk_container_add(GTK_CONTAINER(widget[styleWindow]),layout);

        adjustment=GTK_ADJUSTMENT(gtk_adjustment_new(50,1,100,1,1,20));

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        widget[styleButton]=hildon_gtk_button_new(HILDON_SIZE_AUTO);
  #else
        widget[styleButton]=gtk_button_new();
  #endif
#else
        widget[styleButton]=gtk_button_new();
#endif
        widget[styleToolButton]=gtk_button_new();
        gtk_button_set_focus_on_click(GTK_BUTTON(widget[styleToolButton]),FALSE);
        widget[styleToolbar]=gtk_toolbar_new();
        gtk_button_set_relief(GTK_BUTTON(widget[styleToolButton]),
                              gtk_toolbar_get_relief_style(GTK_TOOLBAR(widget[styleToolbar])));
        gtk_toolbar_append_widget(GTK_TOOLBAR(widget[styleToolbar]),widget[styleToolButton],NULL,NULL);

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        //widget[styleEdit]=gtk_entry_new();
        widget[styleEdit]=hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT);
        //widget[styleEdit]=hildon_entry_new(HILDON_SIZE_AUTO);
  #else
        widget[styleEdit]=gtk_entry_new();
  #endif
#else
        widget[styleEdit]=gtk_entry_new();
#endif
        widget[styleProgressH]=gtk_progress_bar_new();
        gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(widget[styleProgressH]),
                                         GTK_PROGRESS_LEFT_TO_RIGHT);
        widget[styleProgressV]=gtk_progress_bar_new();
        gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(widget[styleProgressV]),
                                         GTK_PROGRESS_BOTTOM_TO_TOP);
        widget[styleHKnob]=gtk_hscrollbar_new(adjustment);
        widget[styleVKnob]=gtk_vscrollbar_new(adjustment);
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        widget[styleCheck]=gtk_check_button_new();
        //widget[styleCheck]=hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
        widget[styleRadio]=hildon_gtk_radio_button_new(HILDON_SIZE_FINGER_HEIGHT,NULL);
        widget[styleRadio2]=hildon_gtk_radio_button_new_from_widget(HILDON_SIZE_FINGER_HEIGHT,
                                                                    GTK_RADIO_BUTTON(widget[styleRadio]));
  #else
        widget[styleCheck]=gtk_check_button_new();
        widget[styleRadio]=gtk_radio_button_new(NULL);
        widget[styleRadio2]=gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(widget[styleRadio]));
  #endif
#else
        widget[styleCheck]=gtk_check_button_new();
        widget[styleRadio]=gtk_radio_button_new(NULL);
        widget[styleRadio2]=gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(widget[styleRadio]));
#endif
        widget[styleTab]=gtk_notebook_new();

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        widget[styleList]=hildon_gtk_tree_view_new(HILDON_UI_MODE_EDIT);
  #else
        widget[styleList]=gtk_tree_view_new();
  #endif
#else
        widget[styleList]=gtk_tree_view_new();
#endif
        // Add column to table and pack cell renderer into tree view column
        column=gtk_tree_view_column_new();
        cellRenderer=gtk_cell_renderer_text_new();
        gtk_tree_view_column_pack_start(column,cellRenderer,TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(widget[styleList]),column);

        // Create list model with one column and one row
        listStore=gtk_list_store_new(1,G_TYPE_STRING);
        gtk_list_store_append(listStore,&treeIter);
        gtk_list_store_set(listStore,&treeIter,0,"Blub",-1);

        // Assign listmodel to table
        gtk_tree_view_set_model(GTK_TREE_VIEW(widget[styleList]),GTK_TREE_MODEL(listStore));
        g_object_unref(listStore);
        gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(widget[styleList])),
                                    GTK_SELECTION_SINGLE);
        gtk_tree_selection_select_iter(gtk_tree_view_get_selection(GTK_TREE_VIEW(widget[styleList])),&treeIter);
        widget[styleColumn]=column->button;

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        list2=GTK_TREE_VIEW(hildon_gtk_tree_view_new(HILDON_UI_MODE_EDIT));
  #else
        list2=GTK_TREE_VIEW(gtk_tree_view_new());
  #endif
#else
        list2=GTK_TREE_VIEW(gtk_tree_view_new());
#endif
        column=gtk_tree_view_column_new();
        gtk_tree_view_append_column(list2,column);
        widget[styleColumnLeft]=column->button;
        column=gtk_tree_view_column_new();
        gtk_tree_view_append_column(list2,column);
        widget[styleColumnMiddle]=column->button;
        column=gtk_tree_view_column_new();
        gtk_tree_view_append_column(list2,column);
        widget[styleColumnRight]=column->button;

        widget[styleComboBox]=gtk_combo_box_new();
        widget[styleEditComboBox]=gtk_combo_box_entry_new();
        widget[styleHLine]=gtk_hseparator_new();
        widget[styleVLine]=gtk_vseparator_new();

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        widget[styleHScale]=hildon_gtk_hscale_new();
        widget[styleVScale]=hildon_gtk_vscale_new();
  #else
        widget[styleHScale]=gtk_hscale_new(adjustment);
        widget[styleVScale]=gtk_vscale_new(adjustment);
  #endif
#else
        widget[styleHScale]=gtk_hscale_new(adjustment);
        widget[styleVScale]=gtk_vscale_new(adjustment);
#endif
        gtk_range_set_inverted(GTK_RANGE(widget[styleVScale]),true);

        widget[styleStatus]=gtk_statusbar_new();
        widget[styleScrolled]=gtk_scrolled_window_new(adjustment,adjustment);
        widget[styleLabel]=gtk_label_new("");
        widget[styleButtonBox]=gtk_hbutton_box_new();
        widget[stylePane]=gtk_hpaned_new();

        widget[styleDummy]=gtk_button_new();
        widget[styleToggleButton]=gtk_button_new();

        widget[styleMenuBar]=gtk_menu_bar_new();
        widget[styleMenuPullDown]=gtk_menu_item_new();

        for (size_t i=styleButton; i<styleMenuPullDown; i++) {
          assert(widget[i]!=NULL);
          if (i!=styleToolButton) {
            gtk_container_add(GTK_CONTAINER(layout),widget[i]);
          }
          gtk_widget_realize(widget[i]);
        }

        gtk_menu_shell_append(GTK_MENU_SHELL(widget[styleMenuBar]),widget[styleMenuPullDown]);

        gtk_widget_grab_focus(widget[styleDummy]);
      }

      GtkWidgets::~GtkWidgets()
      {
        gtk_object_destroy(GTK_OBJECT(widget[styleWindow]));
      }

      GtkWidget* GtkWidgets::GetWidget(GtkStyles style) const
      {
        return widget[style];
      }

      GtkStyle* GtkWidgets::GetStyle(GtkStyles style) const
      {
        return gtk_widget_get_style(widget[style]);
      }

      GtkImage::GtkImage(Display::ImageIndex image,
                         GtkWidget* widget)
        : image(image),
          widget(widget)
      {
        switch (image) {
        case Display::menuCheckImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleCheck],
                                               "indicator-size");
          height=width;
          break;
        case Display::menuRadioImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleRadio],
                                               "indicator-size");
          height=width;
          break;
        case Display::hknobImageIndex:
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
          width=5;
          height=10;
  #else
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                               "slider-width");
          height=width;
  #endif
#else
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                               "slider-width");
          height=width;
#endif
          break;
        case Display::vknobImageIndex:
          height=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                                "slider-width");
          width=height;
          break;
        case Display::arrowLeftImageIndex:
        case Display::arrowRightImageIndex:
        case Display::arrowUpImageIndex:
        case Display::arrowDownImageIndex:
          width=8;
          height=8;
          break;
        case Display::menuSubImageIndex:
#if defined (LUM_HAVE_LIB_HILDON)
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleCheck],
                                               "indicator-size");
          height=width;
#else
          width=6;
          height=7;
#endif
          break;
        case Display::comboImageIndex:
#if GTK_CHECK_VERSION(2,12,0)
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleComboBox],
                                               "arrow-size");
#else
          width=15;
#endif
          height=width;
          break;
        case Display::comboEditButtonImageIndex:
          /* Trick */
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleCheck],
                                               "indicator-size");
          height=width;
          break;
        case Display::scrollLeftImageIndex:
        case Display::scrollRightImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                                "stepper-size");

          height=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                               "slider-width");
          break;
        case Display::scrollUpImageIndex:
        case Display::scrollDownImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                                "slider-width");
          height=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                                "stepper-size");;
          break;
        case Display::comboDividerImageIndex:
          width=2;
          height=2;
          break;
        case Display::treeExpanderImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleList],
                                               "expander-size");
          height=width;
          break;
        case Display::hscaleKnobImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHScale],
                                               "slider-length");
          height=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHScale],
                                               "slider-width");
          break;
        case Display::vscaleKnobImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVScale],
                                               "slider-width");
          height=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVScale],
                                                "slider-length");
          break;
        case Display::leftSliderImageIndex:
        case Display::rightSliderImageIndex:
          width=0;
          height=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHScale],
                                               "slider-width");
          break;
        case Display::topSliderImageIndex:
        case Display::bottomSliderImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVScale],
                                               "slider-width");
          height=0;
          break;
        case Display::menuDividerImageIndex:
          width=4;
          height=2; // TODO: Find out correct value
          break;
        case Display::hpaneHandleImageIndex:
          width=0;
          height=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::stylePane],
                                                "handle-size");
          break;
        case Display::vpaneHandleImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::stylePane],
                                               "handle-size");
          height=0;
          break;
        case Display::radioImageIndex:
        case Display::ledImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleRadio],
                                               "indicator-size");
          height=width;
          break;
        case Display::checkImageIndex:
          width=(size_t)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleCheck],
                                               "indicator-size");
          height=width;
          break;
        }
      }

      size_t GtkImage::GetWidth() const
      {
        return width;
      }

      size_t GtkImage::GetHeight() const
      {
        return height;
      }

      bool GtkImage::GetAlpha() const
      {
        return true;
      }

      unsigned long GtkImage::GetDrawCap() const
      {
        return drawsDisabled|drawsFocused|drawsActivated;
      }

      void GtkImage::DrawScaled(OS::DrawInfo *draw, int x, int y, size_t w, size_t h)
      {
        // We cannot scale images
        std::cerr << "Image of type " << image << " cannot be scaled!" << std::endl;
        assert(false);
      }

      void GtkImage::Draw(OS::DrawInfo *draw, int x, int y)
      {
        DrawStretched(draw,x,y,GetWidth(),GetHeight());
      }

      void GtkImage::DrawStretched(OS::DrawInfo *draw, int x, int y, size_t w, size_t h)
      {
        GdkDrawable  *gtkDraw;
        GdkRectangle rectangle;

        assert(w!=0 && h!=0);

        switch (image) {
        case Display::ledImageIndex:
          if (draw->selected) {
            draw->PushForeground(OS::Display::fillColor);
            draw->FillArc(x,y,w-1,h-1,0*64,360*64);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->shadowColor);
            draw->FillArc(x,y,w-1,h-1,0*64,360*64);
            draw->PopForeground();
          }

          draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->shineColor);
          draw->DrawArc(x+w / 4,y+h / 4,
                       w / 2,h / 2,60*64,90*64);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->shadowColor);
          draw->DrawArc(x,y,w-1,h-1,0*64,360*64);
          draw->PopForeground();
          return;
        default:
          break;
        }


        rectangle.x=0;
        rectangle.y=0;
        rectangle.width=w;
        rectangle.height=h;

        gtkDraw=gdk_pixmap_new(NULL,
                               w,h,
                               OS::display->GetColorDepth());

        if (gtkDraw==NULL) {
          std::cerr << "Cannot create GdkPixmap" << std::endl;
          return;
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          ::cairo_surface_t *srcSurface=dynamic_cast<Cairo::DrawInfo*>(draw)->surface;
          ::cairo_t         *dstCairo=gdk_cairo_create(gtkDraw);

          ::cairo_set_source_surface(dstCairo,srcSurface,-x,-y);
          ::cairo_paint(dstCairo);

          ::cairo_destroy(dstCairo);
        }
#endif
        else {
          assert(true);
        }

        switch (image) {
        case Display::menuCheckImageIndex:
          gtk_paint_check(gtkWidgets->GetStyle(GtkWidgets::styleCheck),
                          gtkDraw,
                          draw->activated ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL,
                          draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleCheck],
                          "checkbutton",
                          0,0,w,h);
          break;
        case Display::menuRadioImageIndex:
          gtk_paint_option(gtkWidgets->GetStyle(GtkWidgets::styleRadio),
                           gtkDraw,
                           draw->activated ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL,
                           draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                           &rectangle,
                           gtkWidgets->widget[draw->selected ? GtkWidgets::styleRadio : GtkWidgets::styleRadio2],
                           "radiobutton",
                           0,0,w,h);
          break;
        case Display::hknobImageIndex:
          gtk_paint_slider(gtkWidgets->GetStyle(GtkWidgets::styleHKnob),
                           gtkDraw,
                           GetGtkRangeStateType(draw,gtkWidgets->widget[GtkWidgets::styleHKnob]),
                           GTK_SHADOW_OUT,
                           &rectangle,
                           gtkWidgets->widget[GtkWidgets::styleHKnob],
                           "slider",
                           0,0,w,h,
                           GTK_ORIENTATION_HORIZONTAL);
          break;
        case Display::vknobImageIndex:
          gtk_paint_slider(gtkWidgets->GetStyle(GtkWidgets::styleVKnob),
                           gtkDraw,
                           GetGtkRangeStateType(draw,gtkWidgets->widget[GtkWidgets::styleVKnob]),
                           GTK_SHADOW_OUT,
                           &rectangle,
                           gtkWidgets->widget[GtkWidgets::styleVKnob],
                           "slider",
                           0,0,w,h,
                           GTK_ORIENTATION_VERTICAL);
          break;
        case Display::arrowLeftImageIndex:
          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                          gtkDraw,
                          GTK_STATE_NORMAL,draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleWindow],
                          "arrow",
                          GTK_ARROW_LEFT,
                          false,
                          0,0,w,h);
          break;
        case Display::arrowRightImageIndex:
          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                          gtkDraw,
                          GTK_STATE_NORMAL,draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleWindow],
                          "arrow",
                          GTK_ARROW_RIGHT,
                          true,
                          0,0,w,h);
          break;
        case Display::arrowUpImageIndex:
          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                          gtkDraw,
                          GTK_STATE_NORMAL,draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleWindow],
                          "arrow",
                          GTK_ARROW_UP,
                          true,
                          0,0,w,h);
          break;
        case Display::arrowDownImageIndex:
          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                          gtkDraw,
                          GTK_STATE_NORMAL,draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleWindow],
                          "arrow",
                          GTK_ARROW_DOWN,
                          true,
                          0,0,w,h);
          break;
        case Display::scrollLeftImageIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleHKnob),
                        gtkDraw,
                        GetGtkStateType(draw),draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleHKnob],
                        GTK_RANGE_GET_CLASS(gtkWidgets->widget[GtkWidgets::styleHKnob])->stepper_detail,
                        0,0,w,h);

          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleHKnob),
                          gtkDraw,
                          GetGtkStateType(draw),draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleHKnob],
                          GTK_RANGE_GET_CLASS(gtkWidgets->widget[GtkWidgets::styleHKnob])->stepper_detail,
                          GTK_ARROW_LEFT,
                          true,
                          (w-w/2)/2,(h-h/2)/2,w/2,h/2);
          break;
        case Display::scrollRightImageIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleHKnob),
                        gtkDraw,
                        GetGtkStateType(draw),draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleHKnob],
                        GTK_RANGE_GET_CLASS(gtkWidgets->widget[GtkWidgets::styleHKnob])->stepper_detail,
                        0,0,w,h);

          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleHKnob),
                          gtkDraw,
                          GetGtkStateType(draw),draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleHKnob],
                          GTK_RANGE_GET_CLASS(gtkWidgets->widget[GtkWidgets::styleHKnob])->stepper_detail,
                          GTK_ARROW_RIGHT,
                          true,
                          (w-w/2)/2,(h-h/2)/2,w/2,h/2);
          break;
        case Display::scrollUpImageIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleVKnob),
                        gtkDraw,
                        GetGtkStateType(draw),draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleVKnob],
                        GTK_RANGE_GET_CLASS(gtkWidgets->widget[GtkWidgets::styleVKnob])->stepper_detail,
                        0,0,w,h);

          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleVKnob),
                          gtkDraw,
                          GetGtkStateType(draw),draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleVKnob],
                          GTK_RANGE_GET_CLASS(gtkWidgets->widget[GtkWidgets::styleVKnob])->stepper_detail,
                          GTK_ARROW_UP,
                          true,
                          (w-w/2)/2,(h-h/2)/2,w/2,h/2);
          break;
        case Display::scrollDownImageIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleVKnob),
                        gtkDraw,
                        GetGtkStateType(draw),draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleVKnob],
                        GTK_RANGE_GET_CLASS(gtkWidgets->widget[GtkWidgets::styleVKnob])->stepper_detail,
                        0,0,w,h);

          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleVKnob),
                          gtkDraw,
                          GetGtkStateType(draw),draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleVKnob],
                          GTK_RANGE_GET_CLASS(gtkWidgets->widget[GtkWidgets::styleVKnob])->stepper_detail,
                          GTK_ARROW_DOWN,
                          true,
                          (w-w/2)/2,(h-h/2)/2,w/2,h/2);
          break;
        case Display::comboImageIndex:
          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleComboBox),
                          gtkDraw,
                          GetGtkStateType(draw),
                          GTK_SHADOW_NONE,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleComboBox],
                          "arrow",
                          GTK_ARROW_DOWN,
                          false,
                          0,0,w,h);
          break;
        case Display::comboEditButtonImageIndex:
          SetFocusIfRequested(draw,GtkWidgets::styleButton);
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleButton),
                        gtkDraw,
                        GetGtkStateType(draw),
                        draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleButton],
                        buttonDetail,
                        0,0,w,h);

          gtk_paint_arrow(gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                          gtkDraw,
                          GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleWindow],
                          NULL,
                          GTK_ARROW_DOWN,
                          true,
                          3,3,w-6,h-6);
          break;
        case Display::comboDividerImageIndex:
          gtk_paint_vline(gtkWidgets->GetStyle(GtkWidgets::styleVLine),
                          gtkDraw,
                          GTK_STATE_NORMAL,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleVLine],
                          NULL,
                          0,h-1,0);
          break;
        case Display::treeExpanderImageIndex:
          gtk_paint_expander(gtkWidgets->GetStyle(GtkWidgets::styleList),
                             gtkDraw,
                             GTK_STATE_NORMAL,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleList],
                             "treeview",
                             w/2,h/2,
                             draw->selected ? GTK_EXPANDER_EXPANDED : GTK_EXPANDER_COLLAPSED);
          break;
        case Display::hscaleKnobImageIndex:
          gtk_paint_slider(gtkWidgets->GetStyle(GtkWidgets::styleHScale),
                           gtkDraw,
                           GetGtkRangeStateType(draw,gtkWidgets->widget[GtkWidgets::styleHScale]),
                           draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                           &rectangle,
                           gtkWidgets->widget[GtkWidgets::styleHScale],
                           "hscale",
                           0,0,w,h,
                           GTK_ORIENTATION_HORIZONTAL);
          break;
        case Display::vscaleKnobImageIndex:
          gtk_paint_slider(gtkWidgets->GetStyle(GtkWidgets::styleVScale),
                           gtkDraw,
                           GetGtkRangeStateType(draw,gtkWidgets->widget[GtkWidgets::styleVScale]),
                           draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                           &rectangle,
                           gtkWidgets->widget[GtkWidgets::styleVScale],
                           "vscale",
                           0,0,w,h,
                           GTK_ORIENTATION_VERTICAL);
          break;
        case Display::leftSliderImageIndex:
          gtk_adjustment_set_value(gtkWidgets->adjustment,100);
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleHScale),
                        gtkDraw,
                        GTK_WIDGET_IS_SENSITIVE(gtkWidgets->widget[GtkWidgets::styleHScale]) ? GTK_STATE_ACTIVE : GTK_STATE_INSENSITIVE,
                        GTK_SHADOW_IN,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleHScale],
                        GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleHScale],
                                                "trough-side-details") ? "trough-lower" :"trough",
                        0,0,w,h);
          break;
        case Display::bottomSliderImageIndex:
          gtk_adjustment_set_value(gtkWidgets->adjustment,100);
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleVScale),
                        gtkDraw,
                        GTK_WIDGET_IS_SENSITIVE(gtkWidgets->widget[GtkWidgets::styleVScale]) ? GTK_STATE_ACTIVE : GTK_STATE_INSENSITIVE,
                        GTK_SHADOW_IN,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleVScale],
                        GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVScale],
                                                "trough-side-details") ? "trough-lower" :"trough",
                        0,0,w,h);
          break;
        case Display::rightSliderImageIndex:
          gtk_adjustment_set_value(gtkWidgets->adjustment,1);
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleHScale),
                        gtkDraw,
                        GTK_WIDGET_IS_SENSITIVE(gtkWidgets->widget[GtkWidgets::styleHScale]) ? GTK_STATE_ACTIVE : GTK_STATE_INSENSITIVE,
                        GTK_SHADOW_IN,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleHScale],
                        GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleHScale],
                                                "trough-side-details") ? "trough-upper" :"trough",
                        0,0,w,h);
          break;
        case Display::topSliderImageIndex:
          gtk_adjustment_set_value(gtkWidgets->adjustment,1);
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleVScale),
                        gtkDraw,
                        GTK_WIDGET_IS_SENSITIVE(gtkWidgets->widget[GtkWidgets::styleVScale]) ? GTK_STATE_ACTIVE : GTK_STATE_INSENSITIVE,
                        GTK_SHADOW_IN,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleVScale],
                        GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVScale],
                                                "trough-side-details") ? "trough-upper" :"trough",
                        0,0,w,h);
          break;
        case Display::menuDividerImageIndex:
          gtk_paint_hline(gtk_widget_get_style(widget),
                          gtkDraw,
                          GTK_STATE_NORMAL,
                          &rectangle,
                          widget,
                          NULL,
                          0,w-1,0);
          break;
        case Display::menuSubImageIndex:
          gtk_paint_arrow(gtk_widget_get_style(widget),
                          gtkDraw,
                          draw->selected ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL,
                          draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          widget,
                          "menuitem",
                          GTK_ARROW_RIGHT,
                          false,
                          0,0,w,h);
          break;
        case Display::hpaneHandleImageIndex:
          gtk_paint_handle(gtkWidgets->GetStyle(GtkWidgets::stylePane),
                          gtkDraw,
                          GTK_STATE_NORMAL,draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::stylePane],
                          NULL,
                          0,0,w,h,
                          GTK_ORIENTATION_HORIZONTAL);
          break;

        case Display::vpaneHandleImageIndex:
          gtk_paint_handle(gtkWidgets->GetStyle(GtkWidgets::stylePane),
                          gtkDraw,
                          GTK_STATE_NORMAL,draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::stylePane],
                          NULL,
                          0,0,w,h,
                          GTK_ORIENTATION_VERTICAL);
          break;
        case Display::radioImageIndex:
          gtk_paint_option(gtkWidgets->GetStyle(GtkWidgets::styleRadio),
                           gtkDraw,
                           draw->activated ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL,
                           draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                           &rectangle,
                           gtkWidgets->widget[draw->selected ? GtkWidgets::styleRadio : GtkWidgets::styleRadio2],
                           "radiobutton",
                           0,0,w,h);
          break;
        case Display::checkImageIndex:
          gtk_paint_check(gtkWidgets->GetStyle(GtkWidgets::styleCheck),
                          gtkDraw,
                          GetGtkStateType(draw),
                          draw->selected ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleCheck],
                          "checkbutton",
                          0,0,w,h);
          break;
        case Display::ledImageIndex:
          break;
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_t *cairo;

          cairo=dynamic_cast<Cairo::DrawInfo*>(draw)->cairo;

          ::cairo_save(cairo);
          ::gdk_cairo_set_source_pixmap(cairo,gtkDraw,x,y);
          ::cairo_rectangle(cairo,x,y,w,h);
          ::cairo_fill(cairo);
          ::cairo_restore(cairo);
        }
#endif
        else {
          assert(true);
        }

        g_object_unref(gtkDraw);
      }


      GtkStockImage::GtkStockImage(const gchar* gtkStockId)
        : gtkStockId(gtkStockId)
      {
        GtkIconSize size=GTK_ICON_SIZE_SMALL_TOOLBAR;

        gtkIconSet=gtk_icon_set_ref(gtk_style_lookup_icon_set(gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                                                              gtkStockId));

        gdkPixbufNormal=gtk_icon_set_render_icon(gtkIconSet,
                                                 gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                                                 GTK_TEXT_DIR_LTR,
                                                 GTK_STATE_NORMAL,
                                                 size,
                                                 gtkWidgets->GetWidget(GtkWidgets::styleWindow),
                                                 NULL);
        gdkPixbufDisabled=gtk_icon_set_render_icon(gtkIconSet,
                                                   gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                                                   GTK_TEXT_DIR_LTR,
                                                   GTK_STATE_INSENSITIVE,
                                                   size,
                                                   gtkWidgets->GetWidget(GtkWidgets::styleWindow),
                                                   NULL);
        gdkPixbufSelected=gtk_icon_set_render_icon(gtkIconSet,
                                                   gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                                                   GTK_TEXT_DIR_LTR,
                                                   GTK_STATE_ACTIVE,
                                                   size,
                                                   gtkWidgets->GetWidget(GtkWidgets::styleWindow),
                                                   NULL);
        gdkPixbufActivated=gtk_icon_set_render_icon(gtkIconSet,
                                                    gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                                                    GTK_TEXT_DIR_LTR,
                                                    GTK_STATE_PRELIGHT,
                                                    size,
                                                    gtkWidgets->GetWidget(GtkWidgets::styleWindow),
                                                    NULL);
      }

      GtkStockImage::~GtkStockImage()
      {
        g_object_unref(gdkPixbufActivated);
        g_object_unref(gdkPixbufSelected);
        g_object_unref(gdkPixbufDisabled);
        g_object_unref(gdkPixbufNormal);
        gtk_icon_set_unref(gtkIconSet);
      }

      size_t GtkStockImage::GetWidth() const
      {
        return gdk_pixbuf_get_width(gdkPixbufNormal);
      }

      size_t GtkStockImage::GetHeight() const
      {
        return gdk_pixbuf_get_height(gdkPixbufNormal);
      }

      bool GtkStockImage::GetAlpha() const
      {
        return true;
      }

      unsigned long GtkStockImage::GetDrawCap() const
      {
        return drawsDisabled|drawsFocused|drawsActivated;
      }

      void GtkStockImage::Draw(OS::DrawInfo *draw, int x, int y)
      {
        GdkDrawable   *gtkDraw;
        GdkRectangle  rectangle;
        GdkPixbuf     *gdkPixbuf;
        GdkGC         *gdkGC;

        if (draw->disabled) {
          gdkPixbuf=gdkPixbufDisabled;
        }
        else if (draw->selected) {
          gdkPixbuf=gdkPixbufSelected;
        }
        else if (draw->activated) {
          gdkPixbuf=gdkPixbufActivated;
        }
        else {
          gdkPixbuf=gdkPixbufNormal;
        }

        rectangle.x=0;
        rectangle.y=0;
        rectangle.width=GetWidth();
        rectangle.height=GetHeight();

        assert(rectangle.width!=0);
        assert(rectangle.height!=0);

        gtkDraw=gdk_pixmap_new(NULL,
                               rectangle.width,rectangle.height,
                               OS::display->GetColorDepth());

        if (gtkDraw==NULL) {
          std::cerr << "Cannot create GdkPixmap" << std::endl;
          return;
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          ::cairo_surface_t *srcSurface=dynamic_cast<Cairo::DrawInfo*>(draw)->surface;
          ::cairo_t         *dstCairo=gdk_cairo_create(gtkDraw);

          ::cairo_set_source_surface(dstCairo,srcSurface,-x,-y);
          ::cairo_paint(dstCairo);

          ::cairo_destroy(dstCairo);
        }
#endif
        else {
          assert(true);
        }

        gdkGC=gdk_gc_new(gtkDraw);
        if (gdkGC!=NULL) {
          gdk_draw_pixbuf(gtkDraw,gdkGC,gdkPixbuf,
                          0,0,
                          0,0,
                          rectangle.width,rectangle.height,
                          GDK_RGB_DITHER_NORMAL,0,0);
          gdk_gc_unref(gdkGC);
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_t *cairo;

          cairo=dynamic_cast<Cairo::DrawInfo*>(draw)->cairo;

          ::cairo_save(cairo);
          ::gdk_cairo_set_source_pixmap(cairo,gtkDraw,x,y);
          ::cairo_rectangle(cairo,
                            x,y,
                            rectangle.width,rectangle.height);
          ::cairo_fill(cairo);
          ::cairo_restore(cairo);
        }
#endif
        else {
          assert(true);
        }

        g_object_unref(gtkDraw);
      }

      void GtkStockImage::DrawScaled(OS::DrawInfo *draw, int x, int y, size_t w, size_t h)
      {
        GdkDrawable   *gtkDraw;
        GdkGC         *gdkGC;
        GdkPixbuf     *gdkPixbuf;
        bool          scaled=false;

        assert(w!=0 && h!=0);

        if (draw->disabled) {
          gdkPixbuf=gdkPixbufDisabled;
        }
        else if (draw->selected) {
          gdkPixbuf=gdkPixbufSelected;
        }
        else if (draw->activated) {
          gdkPixbuf=gdkPixbufActivated;
        }
        else {
          gdkPixbuf=gdkPixbufNormal;
        }

        if (w!=GetWidth() || height!=GetHeight()) {
          scaled=true;

          gdkPixbuf=gdk_pixbuf_scale_simple (gdkPixbuf,
                                             w,h,
                                             GDK_INTERP_BILINEAR);
        }


        gtkDraw=gdk_pixmap_new(NULL,
                               w,h,
                               OS::display->GetColorDepth());

        if (gtkDraw==NULL) {
          std::cerr << "Cannot create GdkPixmap" << std::endl;
          return;
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          ::cairo_surface_t *srcSurface=dynamic_cast<Cairo::DrawInfo*>(draw)->surface;
          ::cairo_t         *dstCairo=gdk_cairo_create(gtkDraw);

          ::cairo_set_source_surface(dstCairo,srcSurface,-x,-y);
          ::cairo_paint(dstCairo);

          ::cairo_destroy(dstCairo);
        }
#endif
        else {
          assert(true);
        }

        gdkGC=gdk_gc_new(gtkDraw);
        if (gdkGC!=NULL) {
          gdk_draw_pixbuf(gtkDraw,gdkGC,gdkPixbuf,
                          0,0,0,0,w,h,
                          GDK_RGB_DITHER_NORMAL,0,0);
          gdk_gc_unref(gdkGC);
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_t *cairo;

          cairo=dynamic_cast<Cairo::DrawInfo*>(draw)->cairo;

          ::cairo_save(cairo);
          ::gdk_cairo_set_source_pixmap(cairo,gtkDraw,x,y);
          ::cairo_rectangle(cairo,x,y,w,h);
          ::cairo_fill(cairo);
          ::cairo_restore(cairo);
        }
#endif
        else {
          assert(true);
        }

          g_object_unref(gtkDraw);

        if (scaled) {
          g_object_unref(gdkPixbuf);
        }
      }

      void GtkStockImage::DrawStretched(OS::DrawInfo *draw, int x, int y, size_t w, size_t h)
      {
        GdkDrawable   *gtkDraw;
        GdkGC         *gdkGC;
        GdkPixbuf     *gdkPixbuf;

        assert(w!=0);
        assert(h!=0);

        if (draw->disabled) {
          gdkPixbuf=gdkPixbufDisabled;
        }
        else if (draw->selected) {
          gdkPixbuf=gdkPixbufSelected;
        }
        else if (draw->activated) {
          gdkPixbuf=gdkPixbufActivated;
        }
        else {
          gdkPixbuf=gdkPixbufNormal;
        }

        gtkDraw=gdk_pixmap_new(NULL,
                               w,h,
                               OS::display->GetColorDepth());

        if (gtkDraw==NULL) {
          std::cerr << "Cannot create GdkPixmap" << std::endl;
          return;
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          ::cairo_surface_t *srcSurface=dynamic_cast<Cairo::DrawInfo*>(draw)->surface;
          ::cairo_t         *dstCairo=gdk_cairo_create(gtkDraw);

          ::cairo_set_source_surface(dstCairo,srcSurface,-x,-y);
          ::cairo_paint(dstCairo);

          ::cairo_destroy(dstCairo);
        }
#endif
        else {
          assert(true);
        }

        gdkGC=gdk_gc_new(gtkDraw);
        if (gdkGC!=NULL) {
          gdk_draw_pixbuf(gtkDraw,gdkGC,gdkPixbuf,
                          0,0,0,0,w,h,
                          GDK_RGB_DITHER_NORMAL,0,0);
          gdk_gc_unref(gdkGC);
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_t *cairo;

          cairo=dynamic_cast<Cairo::DrawInfo*>(draw)->cairo;

          ::cairo_save(cairo);
          ::gdk_cairo_set_source_pixmap(cairo,gtkDraw,x,y);
          ::cairo_rectangle(cairo,x,y,w,h);
          ::cairo_fill(cairo);
          ::cairo_restore(cairo);
        }
#endif
        else {
          assert(true);
        }

        g_object_unref(gtkDraw);
      }

      GtkFill::GtkFill(Display::FillIndex fill)
      : fill(fill)
      {
        switch (fill) {
        case Display::tableBackgroundFillIndex:
        case Display::tableBackground2FillIndex:
        case Display::editComboBackgroundFillIndex:
        case Display::hprogressFillIndex:
        case Display::vprogressFillIndex:
        case Display::tabBackgroundFillIndex:
        case Display::columnEndBackgroundFillIndex:
        case Display::menuStripBackgroundFillIndex:
        case Display::menuPulldownBackgroundFillIndex:
        case Display::toolbarBackgroundFillIndex:
        case Display::statusbarBackgroundFillIndex:
          leftBorder=0;
          rightBorder=0;
          topBorder=0;
          bottomBorder=0;
          transparentBorder=false;
          break;
        case Display::toolbarButtonBackgroundFillIndex:
          GetButtonFrame(gtkWidgets->widget[GtkWidgets::styleToolButton],
                         leftBorder,rightBorder,topBorder,bottomBorder);
          transparentBorder=true;
          break;
        case Display::entryBackgroundFillIndex: {
          GtkBorder *border=NULL;

          leftBorder=0;
          rightBorder=0;
          topBorder=0;
          bottomBorder=0;

          // Frame
          if (gtk_entry_get_has_frame(GTK_ENTRY(gtkWidgets->widget[GtkWidgets::styleEdit]))) {
            leftBorder+=gtkWidgets->widget[GtkWidgets::styleEdit]->style->xthickness;
            rightBorder+=gtkWidgets->widget[GtkWidgets::styleEdit]->style->xthickness;
            topBorder+=gtkWidgets->widget[GtkWidgets::styleEdit]->style->ythickness;
            bottomBorder+=gtkWidgets->widget[GtkWidgets::styleEdit]->style->ythickness;
          }

          // Additional frame for focus
          if (!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleEdit],"interior-focus")) {
            int focusWidth=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleEdit],
                                                  "focus-line-width");

            leftBorder+=focusWidth;
            rightBorder+=focusWidth;
            topBorder+=focusWidth;
            bottomBorder+=focusWidth;
          }

          gtk_widget_style_get(GTK_WIDGET(gtkWidgets->widget[GtkWidgets::styleEdit]),
                               "inner-border",&border,
                               NULL);

          // Additional space between frame(s) and text
          if (border!=NULL) {
            leftBorder+=border->left;
            rightBorder+=border->right;
            topBorder+=border->top;
            bottomBorder+=border->bottom;

            gtk_border_free(border);
          }
          else {
            leftBorder+=2;
            rightBorder+=2;
            topBorder+=2;
            bottomBorder+=2;
          }

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
          topBorder+=4;
          bottomBorder+=5;
  #endif
#endif

          transparentBorder=true;
        }
          break;
        case Display::hknobBoxFillIndex:
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
          leftBorder=0;
          rightBorder=0;
          topBorder=2;
          bottomBorder=2;
          transparentBorder=true;
  #else
          assert(!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                          "trough-under-steppers"));

          leftBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                            "trough-border");
          rightBorder=leftBorder;
          topBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                           "trough-border");
          bottomBorder=topBorder;
  #endif
#else
          assert(!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                          "trough-under-steppers"));

          leftBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                            "trough-border");
          rightBorder=leftBorder;
          topBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                           "trough-border");
          bottomBorder=topBorder;
#endif
          break;
        case Display::vknobBoxFillIndex:
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
          leftBorder=2;
          rightBorder=2;
          topBorder=0;
          bottomBorder=0;
          transparentBorder=true;
  #else
          assert(!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                          "trough-under-steppers"));

          leftBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                            "trough-border");
          rightBorder=leftBorder;
          topBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                           "trough-border");
          bottomBorder=topBorder;
  #endif
#else
          assert(!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                          "trough-under-steppers"));

          leftBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                            "trough-border");
          rightBorder=leftBorder;
          topBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                           "trough-border");
          bottomBorder=topBorder;
#endif
          break;
        case Display::columnBackgroundFillIndex:
        case Display::columnLeftBackgroundFillIndex:
        case Display::columnMiddleBackgroundFillIndex:
        case Display::columnRightBackgroundFillIndex:
          leftBorder=gtkWidgets->widget[GtkWidgets::styleColumn]->style->xthickness;
          rightBorder=leftBorder;
          topBorder=gtkWidgets->widget[GtkWidgets::styleColumn]->style->ythickness;
          bottomBorder=topBorder;
          transparentBorder=true;
          break;
        case Display::hprogressBackgroundFillIndex:
          leftBorder=gtkWidgets->widget[GtkWidgets::styleProgressH]->style->xthickness;
          rightBorder=leftBorder;
          topBorder=gtkWidgets->widget[GtkWidgets::styleProgressH]->style->ythickness;
          bottomBorder=topBorder;
          break;
        case Display::vprogressBackgroundFillIndex:
          leftBorder=gtkWidgets->widget[GtkWidgets::styleProgressV]->style->xthickness;
          rightBorder=leftBorder;
          topBorder=gtkWidgets->widget[GtkWidgets::styleProgressV]->style->ythickness;
          bottomBorder=topBorder;
          break;
        case Display::tooltipWindowBackgroundFillIndex:
          // TODO
          break;
        case Display::graphBackgroundFillIndex:
        case Display::listboxBackgroundFillIndex:
        case Display::boxedBackgroundFillIndex: {
          GtkShadowType shadowType=GTK_SHADOW_NONE;

          g_object_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                         "shadow-type",&shadowType,NULL);

          if (shadowType!=GTK_SHADOW_NONE) {
            gboolean border=false;

#if GTK_CHECK_VERSION(2,12,0)
            gtk_widget_style_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                                 "scrollbars-within-bevel",
                                 &border,NULL);
#endif

            if (!border) {
              leftBorder=gtkWidgets->widget[GtkWidgets::styleScrolled]->style->xthickness;
              rightBorder=leftBorder;
              topBorder=gtkWidgets->widget[GtkWidgets::styleScrolled]->style->ythickness;
              bottomBorder=topBorder;
            }
          }
#if !defined(LUM_HAVE_LIB_HILDON)
          else {
            leftBorder=gtkWidgets->widget[GtkWidgets::styleList]->style->xthickness;
            rightBorder=leftBorder;
            topBorder=gtkWidgets->widget[GtkWidgets::styleList]->style->ythickness;
            bottomBorder=topBorder;
          }
#endif
        }
          transparentBorder=true;
          break;
        case Display::scrolledBackgroundFillIndex: {
          gboolean border=false;

#if GTK_CHECK_VERSION(2,12,0)
          gtk_widget_style_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                               "scrollbars-within-bevel",
                               &border,NULL);
#endif

          if (border) {
            GtkContainer *container=GTK_CONTAINER(gtkWidgets->widget[GtkWidgets::styleScrolled]);

            leftBorder=container->border_width;
            rightBorder=leftBorder;
            topBorder=container->border_width;
            bottomBorder=topBorder;
          }

          transparentBorder=true;
        }
          break;
        case Display::hscrollBackgroundFillIndex:
          if (GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleHKnob],"trough-under-steppers")) {
            leftBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                             "trough-border");
            rightBorder=leftBorder;
            topBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHKnob],
                                             "trough-border");
            bottomBorder=topBorder;
          }
          break;
        case Display::vscrollBackgroundFillIndex:
          if (GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],"trough-under-steppers")) {
            leftBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                              "trough-border");
            rightBorder=leftBorder;
            topBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                             "trough-border");
            bottomBorder=bottomBorder;
          }
          break;
        case Display::labelBackgroundFillIndex:
          leftBorder=1;
          rightBorder=1;
          topBorder=1;
          bottomBorder=1;
          transparentBorder=true;
          //xframe=gtkWidgets->widget[GtkWidgets::styleLabel]->style->xthickness;
          //yframe=gtkWidgets->widget[GtkWidgets::styleLabel]->style->ythickness;
          break;
        case Display::plateBackgroundFillIndex:
          leftBorder=1;
          rightBorder=1;
          topBorder=1;
          bottomBorder=1;
          transparentBorder=true;
          break;
        case Display::tabRiderBackgroundFillIndex:
          leftBorder=2;
          rightBorder=leftBorder;
          topBorder=2;
          bottomBorder=topBorder;
          transparentBorder=true;
          break;
        case Display::backgroundFillIndex:
        case Display::buttonBackgroundFillIndex:
        case Display::positiveButtonBackgroundFillIndex:
        case Display::negativeButtonBackgroundFillIndex:
        case Display::defaultButtonBackgroundFillIndex:
        case Display::comboBackgroundFillIndex:
        case Display::scrollButtonBackgroundFillIndex:
        case Display::menuWindowBackgroundFillIndex:
        case Display::menuEntryBackgroundFillIndex:
        case Display::dialogWindowBackgroundFillIndex:
        case Display::popupWindowBackgroundFillIndex:
          std::cerr << fill << std::endl;
          assert(false);
          break;
        }
      }

      void GtkFill::Draw(OS::DrawInfo* draw,
                         int xOff, int yOff, size_t width, size_t height,
                         int x, int y, size_t w, size_t h)
      {
        ::GdkPixmap    *gtkDraw;
        ::GdkRectangle rectangle;
        size_t         xframe;
        size_t         yframe;

        if (w==0 || h==0) { // TODO: Make this an assertion?
          return;
        }

        // The rectangle defined by x,y,w,h must allways be completely within the rectangle
        // defined by xOff,yOff,width,height!
        assert(x>=xOff);
        assert(y>=yOff);
        assert(x+w<=xOff+width);
        assert(y+h<=yOff+height);

        // Handle fills, that can directly draw to the destination surface...
        switch (fill) {
        case Display::entryBackgroundFillIndex:
          if (draw->selected) {
            if (draw->focused) {
              draw->PushForeground(gtkWidgets->GetStyle(GtkWidgets::styleEdit)->base[GTK_STATE_SELECTED].pixel);
              draw->FillRectangle(x,y,w,h);
              draw->PopForeground();
            }
            else {
              draw->PushForeground(gtkWidgets->GetStyle(GtkWidgets::styleEdit)->base[GTK_STATE_ACTIVE].pixel);
              draw->FillRectangle(x,y,w,h);
              draw->PopForeground();
            }
            return;
          }
          break;
        case Display::plateBackgroundFillIndex:
          draw->PushClip(x,y,w,h);
          draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->blackColor);
          draw->DrawRectangle(xOff,yOff,width,height);
          draw->PopForeground();
          draw->PopClip();
          break;
        case Display::labelBackgroundFillIndex:
          if (!draw->focused) {
            return;
          }
          break;
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        case Display::hknobBoxFillIndex:
        case Display::vknobBoxFillIndex:
          return;
  #endif
#endif
        default:
          break;
        }

        if (fill==Display::tabBackgroundFillIndex) {
          xframe=gtkWidgets->widget[GtkWidgets::styleTab]->style->xthickness;
          yframe=gtkWidgets->widget[GtkWidgets::styleTab]->style->ythickness;
        }
        else {
          xframe=0;
          yframe=0;
        }

        // We cannot draw into the negative area...
        // TODO: Normally this should be fixed by the calling widgets, but we do it
        // here at a central place...

        if (xOff-(int)xframe<0) {
          width+=xOff-(int)xframe;
          xOff=xframe;
        }

        if (yOff-(int)yframe<0) {
          height+=yOff-(int)yframe;
          yOff=yframe;
        }

        if (x<0) {
          w+=x;
          x=0;
        }

        if (y<0) {
          h+=y;
          y=0;
        }

        rectangle.x=x-xOff+xframe;
        rectangle.y=y-yOff+yframe;
        rectangle.width=w;
        rectangle.height=h;

        gtkDraw=gdk_pixmap_new(NULL,
                               width+2*xframe,
                               height+2*yframe,
                               OS::display->GetColorDepth());
        if (gtkDraw==NULL) {
          std::cerr << "Cannot create GdkPixmap" << std::endl;
          return;
        }

        if (transparentBorder) {
#if defined(LUM_HAVE_LIB_CAIRO)
          if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
            ::cairo_surface_t *srcSurface=dynamic_cast<Cairo::DrawInfo*>(draw)->surface;
            ::cairo_t         *dstCairo=gdk_cairo_create(gtkDraw);

            ::cairo_set_source_surface(dstCairo,srcSurface,-x,-y);
            ::cairo_paint(dstCairo);

            ::cairo_destroy(dstCairo);
          }
#endif
          else {
            assert(true);
          }
        }

        switch (fill) {
        case Display::tableBackgroundFillIndex:
          SetFocusIfRequested(draw,GtkWidgets::styleList);
          gtk_paint_flat_box(gtkWidgets->GetStyle(GtkWidgets::styleList),
                             gtkDraw,
                             draw->selected ? GTK_STATE_SELECTED : GTK_STATE_NORMAL,
                             GTK_SHADOW_NONE,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleList],
                             "cell_odd",
                             0,0,width,height);
          break;
        case Display::tableBackground2FillIndex:
          SetFocusIfRequested(draw,GtkWidgets::styleList);
          gtk_paint_flat_box(gtkWidgets->GetStyle(GtkWidgets::styleList),
                             gtkDraw,
                             draw->selected ? GTK_STATE_SELECTED : GTK_STATE_NORMAL,
                             GTK_SHADOW_NONE,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleList],
                             "cell_even",
                             0,0,width,height);
          break;
        case Display::toolbarButtonBackgroundFillIndex:
          DrawButton(gtkWidgets->widget[GtkWidgets::styleToolButton],
                     draw,gtkDraw,&rectangle,0,0,width,height);
          break;
        case Display::entryBackgroundFillIndex: {
          SetFocusIfRequested(draw,GtkWidgets::styleEdit);
          int             frame=0;
          GtkStateType    state=GTK_STATE_NORMAL;
          GtkShadowType   shadowType=GTK_SHADOW_IN;
          bool            interiorFocus=GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleEdit],
                                                                "interior-focus");
          int             hborder=0,vborder=0;

#if GTK_CHECK_VERSION(2,12,0)
          g_object_get(GTK_OBJECT(gtkWidgets->widget[GtkWidgets::styleEdit]),
                       "shadow-type",&shadowType,NULL);
#endif

          if (draw->focused &&
              !draw->disabled &&
              !interiorFocus) {
            frame=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleEdit],
                                         "focus-line-width");
          }

          state=GetGtkStateType(draw);
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
          if (draw->focused &&
              !draw->disabled &&
              GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleEdit],
                                      "state-hint")) {
            state=GTK_STATE_ACTIVE;
          }
  #endif
#endif

          gtk_paint_shadow(gtkWidgets->GetStyle(GtkWidgets::styleEdit),
                           gtkDraw,
                           state,
                           shadowType,
                           &rectangle,
                           gtkWidgets->widget[GtkWidgets::styleEdit],
                           "entry",
                           frame,frame,width-2*frame,height-2*frame);

          if (draw->focused &&
              !draw->disabled &&
              !interiorFocus) {
            gtk_paint_focus(gtkWidgets->GetStyle(GtkWidgets::styleEdit),
                            gtkDraw,
                            state,
                            &rectangle,
                            gtkWidgets->widget[GtkWidgets::styleEdit],
                            "entry",
                            0,0,width,height);
          }

#if !defined(LUM_HAVE_LIB_HILDON) || HILDON_MAJOR_VERSION>2 || (HILDON_MAJOR_VERSION==2 && HILDON_MINOR_VERSION>=1)
          if (gtk_entry_get_has_frame(GTK_ENTRY(gtkWidgets->widget[GtkWidgets::styleEdit]))) {
            hborder+=gtkWidgets->widget[GtkWidgets::styleEdit]->style->xthickness;
            vborder+=gtkWidgets->widget[GtkWidgets::styleEdit]->style->ythickness;
          }
#endif

          hborder+=frame;
          vborder+=frame;

          gtk_paint_flat_box(gtkWidgets->GetStyle(GtkWidgets::styleEdit),
                             gtkDraw,
                             GetGtkStateType(draw),GTK_SHADOW_NONE,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleEdit],
                             "entry_bg",
                             hborder,vborder,width-2*hborder,height-2*vborder);
        }
          break;
        case Display::hprogressBackgroundFillIndex:
          gtk_paint_flat_box(gtkWidgets->GetStyle(GtkWidgets::styleProgressH),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleProgressH],
                             "trough",
                             0,0,width,height);

          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleProgressH),
                        gtkDraw,
                        GTK_STATE_NORMAL,GTK_SHADOW_IN,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleProgressH],
                        "trough",
                        0,0,width,height);
          break;
        case Display::vprogressBackgroundFillIndex:
          gtk_paint_flat_box(gtkWidgets->GetStyle(GtkWidgets::styleProgressV),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleProgressV],
                             "trough",
                             0,0,width,height);

          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleProgressV),
                        gtkDraw,
                        GTK_STATE_NORMAL,GTK_SHADOW_IN,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleProgressV],
                        "trough",
                        0,0,width,height);
          break;
        case Display::hprogressFillIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleProgressH),
                        gtkDraw,
                        GTK_STATE_PRELIGHT,GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleProgressH],
                        "bar",
                        rectangle.x,rectangle.y,rectangle.width,rectangle.height);
          break;
        case Display::vprogressFillIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleProgressV),
                        gtkDraw,
                        GTK_STATE_PRELIGHT,GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleProgressV],
                        "bar",
                        rectangle.x,rectangle.y,rectangle.width,rectangle.height);
          break;
        case Display::hknobBoxFillIndex:
          assert(!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                          "trough-under-steppers"));

          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleHKnob),
                        gtkDraw,
                        GTK_STATE_ACTIVE,GTK_SHADOW_IN,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleHKnob],
                        "trough",
                        0,0,
                        width,height);
          break;
        case Display::vknobBoxFillIndex:
          assert(!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],
                                          "trough-under-steppers"));

          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleVKnob),
                        gtkDraw,
                        GTK_STATE_ACTIVE,GTK_SHADOW_IN,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleVKnob],
                        "trough",
                        0,0,
                        width,height);
          break;
        case Display::tabBackgroundFillIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleTab),
                        gtkDraw,
                        GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleTab],
                        "notebook",
                        0,0,width+2*xframe,height+2*yframe);
          break;
        case Display::tabRiderBackgroundFillIndex:
          gtk_paint_extension(gtkWidgets->GetStyle(GtkWidgets::styleTab),
                              gtkDraw,
                              draw->selected ? GTK_STATE_NORMAL : GTK_STATE_ACTIVE,GTK_SHADOW_OUT,
                              &rectangle,
                              gtkWidgets->widget[GtkWidgets::styleTab],
                              const_cast<gchar*>("tab"),
                              0,0,width,height,
                              GTK_POS_BOTTOM);

          if (draw->focused) {
            /*
            SetFocusIfRequested(draw,GtkWidgets::styleLabel);
            gtk_paint_focus(gtkWidgets->GetStyle(GtkWidgets::styleLabel),
                            gtkDraw,
                            GTK_STATE_NORMAL,
                            &rectangle,
                            gtkWidgets->GetWidget(GtkWidgets::styleLabel),
                            "label",
                            0,0,width,height);*/
          }
          break;
        case Display::columnBackgroundFillIndex:
        case Display::columnLeftBackgroundFillIndex:
        case Display::columnMiddleBackgroundFillIndex:
        case Display::columnRightBackgroundFillIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleButton),
                        gtkDraw,
                        GetGtkStateType(draw),GTK_SHADOW_OUT,
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleColumn],
                        buttonDetail,
                        0,0,width,height);
          break;
        case Display::menuStripBackgroundFillIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleMenuBar),
                        gtkDraw,
                        GTK_STATE_NORMAL,
                        (GtkShadowType)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleMenuBar],
                                                              "shadow-type"),
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleMenuBar],
                        "menubar",
                        0,0,width,height);
          break;
        case Display::menuPulldownBackgroundFillIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleMenuBar),
                        gtkDraw,
                        GTK_STATE_NORMAL,
                        (GtkShadowType)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleMenuBar],
                                                              "shadow-type"),
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleMenuBar],
                        "menubar",
                        0,0,width,height);
          if (draw->selected) {
            gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleMenuPullDown),
                          gtkDraw,
                          GTK_STATE_PRELIGHT,
                          (GtkShadowType)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleMenuPullDown],
                                                                "selected-shadow-type"),
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleMenuPullDown],
                          "menuitem",
                          0,0,width,height);
          }
          break;
        case Display::tooltipWindowBackgroundFillIndex:
          // TODO
          break;
        case Display::toolbarBackgroundFillIndex:
#if defined(LUM_HAVE_LIB_HILDON)
          gtk_paint_flat_box(gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleWindow],
                             NULL,
                             0,0,width,height);
#else
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleToolbar),
                        gtkDraw,
                        GTK_STATE_NORMAL,
                        (GtkShadowType)GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleToolbar],
                                                              "shadow-type"),
                        &rectangle,
                        gtkWidgets->widget[GtkWidgets::styleToolbar],
                        "toolbar",
                        0,0,width,height);
#endif
          break;
        case Display::graphBackgroundFillIndex:
        case Display::listboxBackgroundFillIndex: {
          SetFocusIfRequested(draw,GtkWidgets::styleList);
          gtk_paint_flat_box(gtkWidgets->GetStyle(GtkWidgets::styleList),
                             gtkDraw,
                             draw->selected ? GTK_STATE_SELECTED : GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleList],
                             "cell_odd",
                             0,0,width,height);

          GtkShadowType shadowType=GTK_SHADOW_NONE;

          g_object_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                       "shadow-type",&shadowType,NULL);

          if (shadowType!=GTK_SHADOW_NONE) {
            gboolean border=false;

#if GTK_CHECK_VERSION(2,12,0)
            gtk_widget_style_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                                 "scrollbars-within-bevel",
                                 &border,NULL);
#endif

            if (!border) {
              gtk_paint_shadow(gtkWidgets->GetStyle(GtkWidgets::styleScrolled),
                               gtkDraw,
                               GTK_STATE_NORMAL,shadowType,
                               &rectangle,
                               gtkWidgets->widget[GtkWidgets::styleScrolled],
                               "scrolled_window",
                               0,0,width,height);
            }
          }
          else {
#if !defined(LUM_HAVE_LIB_HILDON)
            gtk_paint_shadow(gtkWidgets->GetStyle(GtkWidgets::styleList),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_IN,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleList],
                             NULL,
                             0,0,width,height);
#endif
          }
          break;
        }
        case Display::boxedBackgroundFillIndex: {
          GtkShadowType shadowType=GTK_SHADOW_NONE;

          g_object_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                       "shadow-type",&shadowType,NULL);

          if (shadowType!=GTK_SHADOW_NONE) {
            gboolean border=false;

#if GTK_CHECK_VERSION(2,12,0)
            gtk_widget_style_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                                 "scrollbars-within-bevel",
                                 &border,NULL);
#endif

            if (!border) {
              gtk_paint_shadow(gtkWidgets->GetStyle(GtkWidgets::styleScrolled),
                               gtkDraw,
                               GTK_STATE_NORMAL,shadowType,
                               &rectangle,
                               gtkWidgets->widget[GtkWidgets::styleScrolled],
                               "scrolled_window",
                               0,0,width,height);
            }
          }
#if !defined(LUM_HAVE_LIB_HILDON)
          else {
            gtk_paint_shadow(gtkWidgets->GetStyle(GtkWidgets::styleList),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_IN,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleList],
                             NULL,
                             0,0,width,height);
          }
#endif
        }
          break;
        case Display::scrolledBackgroundFillIndex: {
          GtkShadowType shadowType=GTK_SHADOW_NONE;

          g_object_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                       "shadow-type",&shadowType,NULL);

          gboolean border=false;

#if GTK_CHECK_VERSION(2,12,0)
          gtk_widget_style_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                               "scrollbars-within-bevel",
                               &border,NULL);
#endif

          if (border) {
            GtkShadowType shadowType=GTK_SHADOW_NONE;

            g_object_get(gtkWidgets->widget[GtkWidgets::styleScrolled],
                         "shadow-type",&shadowType,NULL);

            gtk_paint_shadow(gtkWidgets->GetStyle(GtkWidgets::styleScrolled),
                             gtkDraw,
                             GTK_STATE_NORMAL,shadowType,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleScrolled],
                             "scrolled_window",
                             0,0,width,height);
          }
        }
          break;
        case Display::hscrollBackgroundFillIndex:
          if (GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleHKnob],"trough-under-steppers")) {
            gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleHKnob),
                          gtkDraw,
                          GTK_STATE_ACTIVE,GTK_SHADOW_IN,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleHKnob],
                          "trough",
                          0,0,width,height);
          }
          break;
        case Display::vscrollBackgroundFillIndex:
          if (GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],"trough-under-steppers")) {
            gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleVKnob),
                          gtkDraw,
                          GTK_STATE_ACTIVE,GTK_SHADOW_IN,
                          &rectangle,
                          gtkWidgets->widget[GtkWidgets::styleVKnob],
                          "trough",
                          0,0,width,height);
          }
          break;
        case Display::labelBackgroundFillIndex:
          if (draw->focused) {
            SetFocusIfRequested(draw,GtkWidgets::styleLabel);
            gtk_paint_focus(gtkWidgets->GetStyle(GtkWidgets::styleLabel),
                            gtkDraw,
                            GTK_STATE_NORMAL,
                            &rectangle,
                            gtkWidgets->GetWidget(GtkWidgets::styleLabel),
                            "label",
                            0,0,width,height);
          }
          break;
        case Display::plateBackgroundFillIndex:
          gtk_paint_flat_box(gtkWidgets->GetStyle(GtkWidgets::styleWindow),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             &rectangle,
                             gtkWidgets->widget[GtkWidgets::styleWindow],
                             NULL,
                             1,1,width-2,height-2);
          break;
        case Display::backgroundFillIndex:
        case Display::buttonBackgroundFillIndex:
        case Display::positiveButtonBackgroundFillIndex:
        case Display::negativeButtonBackgroundFillIndex:
        case Display::defaultButtonBackgroundFillIndex:
        case Display::comboBackgroundFillIndex:
        case Display::scrollButtonBackgroundFillIndex:
        case Display::menuEntryBackgroundFillIndex:
        case Display::menuWindowBackgroundFillIndex:
        case Display::dialogWindowBackgroundFillIndex:
          assert(false);
          break;
        default:
          assert(false);
          break;
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          ::cairo_t *cairo=dynamic_cast<Cairo::DrawInfo*>(draw)->cairo;

          ::cairo_save(cairo);
          ::gdk_cairo_set_source_pixmap(cairo,gtkDraw,xOff-xframe,yOff-yframe);
          ::cairo_rectangle(cairo,x,y,w,h);
          ::cairo_fill(cairo);
          ::cairo_restore(cairo);
        }
#endif
        else {
          assert(true);
        }

        g_object_unref(gtkDraw);
      }

      class GtkWidgetFill : public Fill
      {
      public:
        virtual void DrawFill(OS::DrawInfo* draw,
                              ::GdkPixmap* gtkDraw,
                              ::GdkRectangle* rectangle,
                              size_t width,
                              size_t height) = 0;

        void Draw(OS::DrawInfo* draw,
                  int xOff, int yOff, size_t width, size_t height,
                  int x, int y, size_t w, size_t h);
      };

      void GtkWidgetFill::Draw(OS::DrawInfo* draw,
                               int xOff, int yOff, size_t width, size_t height,
                               int x, int y, size_t w, size_t h)
      {
        ::GdkPixmap    *gtkDraw;
        ::GdkRectangle rectangle;
        size_t         xframe;
        size_t         yframe;

        if (w==0 || h==0) { // TODO: Make this an assertion?
          return;
        }

        // The rectangle defined by x,y,w,h must allways be completely within the rectangle
        // defined by xOff,yOff,width,height!
        assert(x>=xOff && y>=yOff && x+w<=xOff+width && y+h<=yOff+height);

        xframe=0;
        yframe=0;

        // We cannot draw into the negative area...
        // TODO: Normally this should be fixed by the calling widgets, but we do it
        // here at a central place...

        if (xOff-(int)xframe<0) {
          width+=xOff-(int)xframe;
          xOff=xframe;
        }

        if (yOff-(int)yframe<0) {
          height+=yOff-(int)yframe;
          yOff=yframe;
        }

        if (x<0) {
          w+=x;
          x=0;
        }

        if (y<0) {
          h+=y;
          y=0;
        }

        rectangle.x=x-xOff+xframe;
        rectangle.y=y-yOff+yframe;
        rectangle.width=w;
        rectangle.height=h;

        gtkDraw=gdk_pixmap_new(NULL,
                               width+2*xframe,
                               height+2*yframe,
                               OS::display->GetColorDepth());
        if (gtkDraw==NULL) {
          std::cerr << "Cannot create GdkPixmap" << std::endl;
          return;
        }

        if (transparentBorder) {
#if defined(LUM_HAVE_LIB_CAIRO)
          if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
            ::cairo_surface_t *srcSurface=dynamic_cast<Cairo::DrawInfo*>(draw)->surface;
            ::cairo_t         *dstCairo=gdk_cairo_create(gtkDraw);

            ::cairo_set_source_surface(dstCairo,srcSurface,-x,-y);
            ::cairo_paint(dstCairo);

            ::cairo_destroy(dstCairo);
          }
#endif
          else {
            assert(true);
          }
        }

        DrawFill(draw,gtkDraw,&rectangle,width,height);

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          ::cairo_t *cairo=dynamic_cast<Cairo::DrawInfo*>(draw)->cairo;

          ::cairo_save(cairo);
          ::gdk_cairo_set_source_pixmap(cairo,gtkDraw,xOff-xframe,yOff-yframe);
          ::cairo_rectangle(cairo,x,y,w,h);
          ::cairo_fill(cairo);
          ::cairo_restore(cairo);
        }
#endif
        else {
          assert(true);
        }

        g_object_unref(gtkDraw);
      }

      class PopupFill : public GtkWidgetFill
      {
      private:
        GtkWidget *popup;

      public:
        PopupFill(GtkWidget* popup)
         : popup(popup)
        {
/*#if defined(LUM_HAVE_LIB_HILDON)
          leftBorder=GTK_CONTAINER(popup)->border_width+
                     popup->style->xthickness+4;
          rightBorder=leftBorder;
          topBorder=GTK_CONTAINER(popup)->border_width+
                    popup->style->ythickness+4;
          bottomBorder=topBorder;
#else*/
          leftBorder=popup->style->xthickness;
          rightBorder=leftBorder;
          topBorder=popup->style->ythickness;
          bottomBorder=topBorder;
//#endif
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
#if defined(LUM_HAVE_LIB_HILDON)
          gtk_paint_box(gtk_widget_get_style(popup),
                        gtkDraw,
                        GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                        rectangle,
                        popup,
                        "menu",
                        0,0,width,height);
#else
          gtk_paint_flat_box(gtk_widget_get_style(popup),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             rectangle,
                             popup,
                             "base",
                             0,0,width,height);
#endif
        }
      };

      class MenuFill : public GtkWidgetFill
      {
      private:
        GtkWidget *menu;

      public:
        MenuFill(GtkWidget* menu)
         : menu(menu)
        {
          guint verticalPadding, horizontalPadding;

          gtk_widget_style_get(menu,
                               "vertical-padding",&verticalPadding,
                               "horizontal-padding",&horizontalPadding,
                               NULL);

          leftBorder=GTK_CONTAINER(menu)->border_width+
                     menu->style->xthickness+horizontalPadding;
          rightBorder=leftBorder;
          topBorder=GTK_CONTAINER(menu)->border_width+
                    menu->style->ythickness+verticalPadding;
          bottomBorder=topBorder;
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
          gtk_paint_flat_box(gtk_widget_get_style(menu),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             rectangle,
                             menu,
                             "menu",
                             0,0,width,height);

          gtk_paint_box(gtk_widget_get_style(menu),
                        gtkDraw,
                        GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                        rectangle,
                        menu,
                        "menu",
                        0,0,width,height);
        }
      };

      class MenuItemFill : public GtkWidgetFill
      {
      private:
        GtkWidget *menuItem;

      public:
        MenuItemFill(GtkWidget* menuItem)
         : menuItem(menuItem)
        {
          transparentBorder=true;
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
          if (draw->selected) {
            gtk_paint_box(gtk_widget_get_style(menuItem),
                          gtkDraw,
                          GTK_STATE_PRELIGHT,
                          (GtkShadowType)GetWidgetStyleIntValue(menuItem,
                                                                "selected-shadow-type"),
                          rectangle,
                          menuItem,
                          "menuitem",
                          0,0,width,height);
          }
        }
      };

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
      class HildonButtonFill : public GtkWidgetFill
      {
      private:
        GtkWidget *fingerButton;
        GtkWidget *thumbButton;
        GtkWidget *otherButton;

      public:
        HildonButtonFill(GtkWidget *window, GtkWidget* layout)
        {
          fingerButton=hildon_gtk_button_new(HILDON_SIZE_FINGER_HEIGHT);
          thumbButton=hildon_gtk_button_new(HILDON_SIZE_THUMB_HEIGHT);
          otherButton=hildon_gtk_button_new(HILDON_SIZE_AUTO);

          gtk_container_add(GTK_CONTAINER(layout),fingerButton);
          gtk_widget_realize(fingerButton);
          gtk_container_add(GTK_CONTAINER(layout),thumbButton);
          gtk_widget_realize(thumbButton);
          gtk_container_add(GTK_CONTAINER(layout),otherButton);
          gtk_widget_realize(otherButton);

          GetButtonFrame(fingerButton,
                         leftBorder,rightBorder,topBorder,bottomBorder);

          transparentBorder=true;
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
          if (height==70) {
            DrawButton(fingerButton,
                       draw,gtkDraw,
                       rectangle,
                       0,0,width,height);
          }
          else if (height==105) {
            DrawButton(thumbButton,
                       draw,gtkDraw,
                       rectangle,
                       0,0,width,height);
          }
          else {
            DrawButton(otherButton,
                       draw,gtkDraw,
                       rectangle,
                       0,0,width,height);
          }
        }
      };
  #endif
#endif

      class GtkButtonFill : public GtkWidgetFill
      {
      private:
        GtkWidget *button;

      public:
        GtkButtonFill(GtkWidget *window, GtkWidget* layout)
        {
          button=gtk_button_new();

          gtk_container_add(GTK_CONTAINER(layout),button);
          gtk_widget_realize(button);

          GetButtonFrame(button,
                         leftBorder,rightBorder,topBorder,bottomBorder);

          transparentBorder=true;
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
          DrawButton(button,
                     draw,gtkDraw,rectangle,0,0,width,height);
        }
      };

      class DialogFill : public GtkWidgetFill
      {
      private:
        GtkWidget *window;

      public:
        DialogFill(GtkWidget *window)
         : window(window)
        {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
          // Hardcoded values from Fremantle master layout guide
          leftBorder=16;
          rightBorder=16;
          topBorder=4;
          bottomBorder=8;
  #else
          {
            GtkBorder *border=NULL;

            gtk_widget_style_get(window,"borders",&border,NULL);

            if (border!=NULL) {
              leftBorder=border->left;
              rightBorder=border->right;
              topBorder=border->top;
              bottomBorder=border->bottom;

              gtk_border_free(border);
            }
          }
  #endif
#else
          leftBorder=0;
          rightBorder=leftBorder;
          topBorder=0;
          bottomBorder=topBorder;
#endif
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
          gtk_paint_flat_box(gtk_widget_get_style(window),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             rectangle,
                             window,
                             NULL,
                             0,0,width,height);

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
  #else
          if (leftBorder>0) {
            gtk_paint_box(gtk_widget_get_style(window),
                          gtkDraw,
                          GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                          rectangle,
                          window,
                          "left-border",
                          0,topBorder,leftBorder,height-topBorder-bottomBorder);
          }
          if (rightBorder>0) {
            gtk_paint_box(gtk_widget_get_style(window),
                          gtkDraw,
                          GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                          rectangle,
                          window,
                          "right-border",
                          width-rightBorder,topBorder,rightBorder,height-topBorder-bottomBorder);
          }
          if (topBorder>0) {
            gtk_paint_box(gtk_widget_get_style(window),
                          gtkDraw,
                          GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                          rectangle,
                          window,
                          "top-border",
                          0,0,width,topBorder);
          }
          if (bottomBorder>0) {
            gtk_paint_box(gtk_widget_get_style(window),
                          gtkDraw,
                          GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                          rectangle,
                          window,
                          "bottom-border",
                          0,height-bottomBorder,width,bottomBorder);
          }
  #endif
#endif
        }
      };

      class BackgroundFill : public GtkWidgetFill
      {
      private:
        GtkWidget *window;

      public:
        BackgroundFill(GtkWidget *window)
         : window(window)
        {
          leftBorder=0;
          rightBorder=leftBorder;
          topBorder=0;
          bottomBorder=topBorder;
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
          gtk_paint_flat_box(gtk_widget_get_style(window),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             rectangle,
                             window,
                             NULL,
                             0,0,width,height);
        }
      };

      class EmptyFill : public GtkWidgetFill
      {
      public:
        EmptyFill()
        {
          leftBorder=0;
          rightBorder=leftBorder;
          topBorder=0;
          bottomBorder=topBorder;
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
          // no code
        }
      };

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
      class AppMenuFill : public GtkWidgetFill
      {
      private:
        GtkWidget *menu;

      public:
        AppMenuFill()
        {
          guint innerBorder=16;

          menu=hildon_app_menu_new();
          gtk_widget_realize(menu);

          gtk_widget_style_get(menu,"inner-border",&innerBorder,NULL);

          leftBorder=innerBorder;
          rightBorder=innerBorder;
          topBorder=innerBorder;
          bottomBorder=innerBorder;
        }

        ~AppMenuFill()
        {
          gtk_object_destroy(GTK_OBJECT(menu));
        }

        void DrawFill(OS::DrawInfo* draw,
                      ::GdkPixmap* gtkDraw,
                      ::GdkRectangle* rectangle,
                      size_t width,
                      size_t height)
        {
          gtk_paint_flat_box(gtk_widget_get_style(menu),
                             gtkDraw,
                             GTK_STATE_NORMAL,GTK_SHADOW_NONE,
                             rectangle,
                             menu,
                             NULL,
                             0,0,width,height);
        }
      };

  #endif
#endif
      GtkFrame::GtkFrame(Display::FrameIndex frame)
      : frame(frame)
      {
        alpha=true;
        switch (frame) {
        case Display::tabFrameIndex:
          leftBorder=gtkWidgets->widget[GtkWidgets::styleTab]->style->xthickness;
          rightBorder=leftBorder;
          topBorder=gtkWidgets->widget[GtkWidgets::styleTab]->style->ythickness;
          bottomBorder=topBorder;
          break;
        case Display::hscaleFrameIndex:
          leftBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHScale],
                                            "trough-border");
          rightBorder=leftBorder;
          topBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleHScale],
                                           "trough-border");
          bottomBorder=topBorder;
          break;
        case Display::vscaleFrameIndex:
          leftBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVScale],
                                            "trough-border");
          rightBorder=leftBorder;
          topBorder=GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleVScale],
                                           "trough-border");
          bottomBorder=topBorder;
          break;
        case Display::focusFrameIndex:
          leftBorder=1;
          rightBorder=1;
          topBorder=1;
          bottomBorder=1;
          break;
        case Display::statuscellFrameIndex:
          leftBorder=gtkWidgets->widget[GtkWidgets::styleStatus]->style->xthickness;
          rightBorder=leftBorder;
          topBorder=gtkWidgets->widget[GtkWidgets::styleStatus]->style->ythickness;
          bottomBorder=topBorder;
          break;
        case Display::valuebarFrameIndex:
          leftBorder=1;
          rightBorder=1;
          topBorder=1;
          bottomBorder=1;
          break;
        case Display::groupFrameIndex:
          leftBorder=2;
          rightBorder=2;
          topBorder=2;
          bottomBorder=2;
          break;
        }

        minWidth=leftBorder+rightBorder;
        minHeight=topBorder+bottomBorder;

        if (minWidth==0 && minHeight==0) {
          alpha=false;
        }
      }

      void GtkFrame::Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h)
      {
        GdkDrawable       *gtkDraw;

        if (minWidth==0 && minHeight==0) {
          return;
        }

        if (x<0 && -x>(int)w) {
          return;
        }

        if (y<0 && -y>(int)h) {
          return;
        }

        if (x<0) {
          w+=x;
          x=0;
        }

        if (y<0) {
          h+=y;
          y=0;
        }

        switch (frame) {
        case Display::valuebarFrameIndex:
          draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->shineColor);
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-1,y);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->shadowColor);
          draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->PopForeground();
          return;
        case Display::groupFrameIndex:
          {
            int top;

            if (HasGap()) {
              top=y+gh / 2;
            }
            else {
              top=y;
            }
            draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->shadowColor);
            draw->DrawLine(x,y+h-1,x,top);
            draw->DrawLine(x+w-2,top+1,x+w-2,y+h-2);
            draw->DrawLine(x+1,y+h-2,x+w-2,y+h-2);
            if (HasGap()) {
              draw->DrawLine(x,top,x+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)-1,top);
              draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)+gw+1,top,x+w-1,top);
              draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)+gw,top,
                             x+3*OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)+gw,top+1);
            }
            else {
              draw->DrawLine(x,top,x+w-1,top);
            }
            draw->PopForeground();

            draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->shineColor);
            draw->DrawLine(x+1,y+h-2,x+1,top+1);
            draw->DrawLine(x+w-1,top+1,x+w-1,y+h-1);
            draw->DrawLine(x+1,y+h-1,x+w-2,y+h-1);
            if (HasGap()) {
              draw->DrawLine(x+2,top+1,x+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)-1,top+1);
              draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)+gw+1,top+1,x+w-2,top+1);
              draw->DrawLine(x+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject),top,x+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject),top+1);
            }
            else {
              draw->DrawLine(x+2,top+1,x+w-2,top+1);
            }
            draw->PopForeground();
          }
          return;
        default:
          break;
        }

        gtkDraw=gdk_pixmap_new(NULL,
                               w,h,
                               OS::display->GetColorDepth());

        if (gtkDraw==NULL) {
          std::cerr << "Cannot create GdkPixmap" << std::endl;
          return;
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          ::cairo_surface_t *srcSurface=dynamic_cast<Cairo::DrawInfo*>(draw)->surface;
          ::cairo_t         *dstCairo=gdk_cairo_create(gtkDraw);

          ::cairo_set_source_surface(dstCairo,srcSurface,-x,-y);
          ::cairo_paint(dstCairo);

          ::cairo_destroy(dstCairo);
        }
#endif
        else {
          assert(true);
        }

        switch (frame) {
        case Display::tabFrameIndex:
          gtk_paint_box_gap(gtkWidgets->GetStyle(GtkWidgets::styleTab),
                            gtkDraw,
                            GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                            NULL,
                            gtkWidgets->widget[GtkWidgets::styleTab],
                            const_cast<gchar*>("notebook"),
                            0,0,w,h,
                            GTK_POS_TOP,
                            gx,gw);
          break;
        case Display::hscaleFrameIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleHScale),
                        gtkDraw,
                        GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                        NULL,
                        gtkWidgets->widget[GtkWidgets::styleHScale],
                        "stepper",
                        0,0,w,h);
          break;
        case Display::vscaleFrameIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleVScale),
                        gtkDraw,
                        GTK_STATE_NORMAL,GTK_SHADOW_OUT,
                        NULL,
                        gtkWidgets->widget[GtkWidgets::styleVScale],
                        "trough",
                        0,0,w,h);
          break;
        case Display::focusFrameIndex:
          SetFocusIfRequested(draw,GtkWidgets::styleButton);
          gtk_paint_focus(gtkWidgets->GetStyle(GtkWidgets::styleButton),
                          gtkDraw,
                          GTK_STATE_NORMAL,
                          NULL,
                          gtkWidgets->GetWidget(GtkWidgets::styleButton),
                          "button",
                          0,0,w,h);
          break;
        case Display::statuscellFrameIndex:
          gtk_paint_box(gtkWidgets->GetStyle(GtkWidgets::styleStatus),
                        gtkDraw,
                        GTK_STATE_NORMAL,
                        (GtkShadowType)GetWidgetStyleIntValue(gtkWidgets->GetWidget(GtkWidgets::styleStatus),"shadow-type"),
                        NULL,
                        gtkWidgets->widget[GtkWidgets::styleStatus],
                        "statusbar",
                        0,0,w,h);
          break;
        default:
          assert(false);
          break;
        }

        draw->PushClipBegin(x,y,w,h);
        draw->SubClipRegion(x+leftBorder,y+topBorder,w-minWidth,h-minHeight);
        draw->PushClipEnd();

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          ::cairo_t *cairo;

          cairo=dynamic_cast<Cairo::DrawInfo*>(draw)->cairo;

          ::cairo_save(cairo);
          ::gdk_cairo_set_source_pixmap(cairo,gtkDraw,x,y);
          ::cairo_rectangle(cairo,x,y,w,h);
          ::cairo_fill(cairo);
          ::cairo_restore(cairo);
        }
#endif
        else {
          assert(true);
        }

        draw->PopClip();

        g_object_unref(gtkDraw);

      }

      int GetWidgetStyleIntValue(GtkWidget *widget, const char* name)
      {
        GValue gvalue;

        memset(&gvalue,0,sizeof(GValue));
        g_value_init(&gvalue,G_TYPE_INT);
        gtk_widget_style_get_property(widget,name,&gvalue);
        return g_value_get_int(&gvalue);
      }

      float GetWidgetStyleFloatValue(GtkWidget *widget, const char* name)
      {
        GValue gvalue;

        memset(&gvalue,0,sizeof(GValue));
        g_value_init(&gvalue,G_TYPE_FLOAT);
        gtk_widget_style_get_property(widget,name,&gvalue);
        return g_value_get_float(&gvalue);
      }

      bool GetWidgetStyleBoolValue(GtkWidget *widget, const char* name)
      {
        gboolean value=true;

        gtk_widget_style_get(widget,name,&value,NULL);
        return value;
      }

      void GetWidgetStyleBorderValue(GtkWidget *widget, const char* name,
                                     int& left, int&right, int &top, int& bottom)
      {
        GtkBorder *border=NULL;

        gtk_widget_style_get(widget,name,&border,NULL);
        if (border!=NULL) {
          left=border->left;
          right=border->right;
          top=border->top;
          bottom=border->bottom;
          gtk_border_free(border);
        }
      }

      bool GetSettingsBoolValue(const char* name)
      {
        GValue      gvalue;
        GtkSettings *settings;

        settings=gtk_settings_get_default();

        assert(settings!=NULL);

        memset(&gvalue,0,sizeof(GValue));
        g_value_init(&gvalue,G_TYPE_BOOLEAN);
        g_object_get_property(G_OBJECT(settings),name,&gvalue);
        return g_value_get_boolean(&gvalue);
      }

      GtkTheme::GtkTheme(OS::Display *display)
      : OS::Base::Theme(display)
      {
        gtkWidgets=new GtkWidgets();

        popup=gtk_window_new(GTK_WINDOW_POPUP);
        gtk_widget_set_name(popup,"gtk-combobox-popup-window");
        gtk_window_set_resizable(GTK_WINDOW(popup),false);
        gtk_widget_realize(popup);

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        menu=hildon_gtk_menu_new();
  #else
        menu=gtk_menu_new();
  #endif
#else
        menu=gtk_menu_new();
#endif
        menuItem=gtk_menu_item_new();
        gtk_widget_realize(menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuItem);

#if defined(LUM_HAVE_LIB_HILDON)
        window=hildon_window_new();
#else
        window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif
        gtk_widget_realize(window);

        layout=gtk_fixed_new();
        gtk_container_add(GTK_CONTAINER(window),layout);

        color[Display::backgroundColor]=gtkWidgets->widget[GtkWidgets::styleWindow]->style->bg[GTK_STATE_NORMAL].pixel;
        color[Display::textColor]=gtkWidgets->widget[GtkWidgets::styleWindow]->style->text[GTK_STATE_NORMAL].pixel;
        color[Display::tableTextColor]=gtkWidgets->widget[GtkWidgets::styleList]->style->text[GTK_STATE_NORMAL].pixel;
        color[Display::textSelectColor]=gtkWidgets->widget[GtkWidgets::styleButton]->style->text[GTK_STATE_SELECTED].pixel;
        color[Display::textDisabledColor]=gtkWidgets->widget[GtkWidgets::styleButton]->style->text[GTK_STATE_INSENSITIVE].pixel;
        color[Display::fillColor]=gtkWidgets->widget[GtkWidgets::styleEdit]->style->base[GTK_STATE_SELECTED].pixel;
        color[Display::fillTextColor]=gtkWidgets->widget[GtkWidgets::styleList]->style->text[GTK_STATE_SELECTED].pixel;
        color[Display::graphScaleColor]=gtkWidgets->widget[GtkWidgets::styleWindow]->style->text[GTK_STATE_NORMAL].pixel;
        color[Display::blackColor]=gtkWidgets->widget[GtkWidgets::styleWindow]->style->black.pixel;
        color[Display::whiteColor]=gtkWidgets->widget[GtkWidgets::styleWindow]->style->white.pixel;
        color[Display::tabTextColor]=gtkWidgets->widget[GtkWidgets::styleTab]->style->text[GTK_STATE_NORMAL].pixel;
        color[Display::tabTextSelectColor]=gtkWidgets->widget[GtkWidgets::styleTab]->style->text[GTK_STATE_NORMAL].pixel; // Common for all themes?
        color[Display::editTextColor]=gtkWidgets->widget[GtkWidgets::styleEdit]->style->text[GTK_STATE_NORMAL].pixel;
        color[Display::editTextFillColor]=gtkWidgets->widget[GtkWidgets::styleEdit]->style->text[GTK_STATE_SELECTED].pixel; // Common for all themes?
#if defined(LUM_HAVE_LIB_HILDON)
        color[Display::textSmartAColor]=color[Display::blackColor];
        color[Display::textSmartBColor]=color[Display::fillColor];
#else
        color[Display::textSmartAColor]=color[Display::blackColor];
        color[Display::textSmartBColor]=color[Display::whiteColor];
#endif

        display->AllocateNamedColor("light yellow",helpBackgroundColor);

        whiteColor=gtkWidgets->widget[GtkWidgets::styleWindow]->style->white.pixel;
        blackColor=gtkWidgets->widget[GtkWidgets::styleWindow]->style->black.pixel;
        shineColor=gtkWidgets->widget[GtkWidgets::styleWindow]->style->light[GTK_STATE_NORMAL].pixel;
        shadowColor=gtkWidgets->widget[GtkWidgets::styleWindow]->style->dark[GTK_STATE_NORMAL].pixel;

        fill[Display::backgroundFillIndex]=new BackgroundFill(window);

        fill[Display::tableBackgroundFillIndex]=new GtkFill(Display::tableBackgroundFillIndex);
        fill[Display::tableBackground2FillIndex]=new GtkFill(Display::tableBackground2FillIndex);
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        fill[Display::buttonBackgroundFillIndex]=new HildonButtonFill(window,layout);
  #else
        fill[Display::buttonBackgroundFillIndex]=new GtkButtonFill(window,layout);
  #endif
#else
        fill[Display::buttonBackgroundFillIndex]=new GtkButtonFill(window,layout);
#endif
        fill[Display::positiveButtonBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::negativeButtonBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::defaultButtonBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::toolbarButtonBackgroundFillIndex]=new GtkFill(Display::toolbarButtonBackgroundFillIndex);
        fill[Display::scrollButtonBackgroundFillIndex]=new EmptyFill();
        fill[Display::comboBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];//new GtkFill(Display::comboBackgroundFillIndex);
        fill[Display::editComboBackgroundFillIndex]=NULL;

        fill[Display::entryBackgroundFillIndex]=new GtkFill(Display::entryBackgroundFillIndex);
        fill[Display::hprogressBackgroundFillIndex]=new GtkFill(Display::hprogressBackgroundFillIndex);
        fill[Display::hprogressFillIndex]=new GtkFill(Display::hprogressFillIndex);
        fill[Display::vprogressBackgroundFillIndex]=new GtkFill(Display::vprogressBackgroundFillIndex);
        fill[Display::vprogressFillIndex]=new GtkFill(Display::vprogressFillIndex);
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
          fill[Display::hknobBoxFillIndex]=new GtkFill(Display::hknobBoxFillIndex);
          fill[Display::vknobBoxFillIndex]=new GtkFill(Display::vknobBoxFillIndex);
  #else
        if (!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],"trough-under-steppers")) {
          fill[Display::hknobBoxFillIndex]=new GtkFill(Display::hknobBoxFillIndex);
          fill[Display::vknobBoxFillIndex]=new GtkFill(Display::vknobBoxFillIndex);
        }
        else {
          fill[Display::hknobBoxFillIndex]=NULL;
          fill[Display::vknobBoxFillIndex]=NULL;
        }
  #endif
#else
        if (!GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleVKnob],"trough-under-steppers")) {
          fill[Display::hknobBoxFillIndex]=new GtkFill(Display::hknobBoxFillIndex);
          fill[Display::vknobBoxFillIndex]=new GtkFill(Display::vknobBoxFillIndex);
        }
        else {
          fill[Display::hknobBoxFillIndex]=NULL;
          fill[Display::vknobBoxFillIndex]=NULL;
        }
#endif

        fill[Display::tabBackgroundFillIndex]=new GtkFill(Display::tabBackgroundFillIndex);
        fill[Display::tabRiderBackgroundFillIndex]=new GtkFill(Display::tabRiderBackgroundFillIndex);
        fill[Display::columnBackgroundFillIndex]=new GtkFill(Display::columnBackgroundFillIndex);
        fill[Display::columnLeftBackgroundFillIndex]=new GtkFill(Display::columnLeftBackgroundFillIndex);
        fill[Display::columnMiddleBackgroundFillIndex]=new GtkFill(Display::columnMiddleBackgroundFillIndex);
        fill[Display::columnRightBackgroundFillIndex]=new GtkFill(Display::columnRightBackgroundFillIndex);
        fill[Display::columnEndBackgroundFillIndex]=fill[Display::backgroundFillIndex];
#if defined(LUM_HAVE_LIB_HILDON)
        fill[Display::menuStripBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::menuPulldownBackgroundFillIndex]=fill[Display::backgroundFillIndex];
#else
        fill[Display::menuStripBackgroundFillIndex]=new GtkFill(Display::menuStripBackgroundFillIndex);
        fill[Display::menuPulldownBackgroundFillIndex]=new GtkFill(Display::menuPulldownBackgroundFillIndex);
#endif

        fill[Display::menuEntryBackgroundFillIndex]=new MenuItemFill(menuItem);
        fill[Display::menuWindowBackgroundFillIndex]=new MenuFill(menu);
        fill[Display::popupWindowBackgroundFillIndex]=new PopupFill(popup);
        fill[Display::dialogWindowBackgroundFillIndex]=new DialogFill(window);
        fill[Display::tooltipWindowBackgroundFillIndex]=new GtkFill(Display::tooltipWindowBackgroundFillIndex); // TODO
        fill[Display::listboxBackgroundFillIndex]=new GtkFill(Display::listboxBackgroundFillIndex);

        if (UseScrolledBackgroundFill()) {
          fill[Display::scrolledBackgroundFillIndex]=new GtkFill(Display::scrolledBackgroundFillIndex);
        }
        else {
          fill[Display::scrolledBackgroundFillIndex]=NULL;
        }

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        fill[Display::hscrollBackgroundFillIndex]=NULL;
        fill[Display::vscrollBackgroundFillIndex]=NULL;
  #else
        fill[Display::hscrollBackgroundFillIndex]=new GtkFill(Display::hscrollBackgroundFillIndex);
        fill[Display::vscrollBackgroundFillIndex]=new GtkFill(Display::vscrollBackgroundFillIndex);
  #endif
#else
        fill[Display::hscrollBackgroundFillIndex]=new GtkFill(Display::hscrollBackgroundFillIndex);
        fill[Display::vscrollBackgroundFillIndex]=new GtkFill(Display::vscrollBackgroundFillIndex);
#endif
        fill[Display::plateBackgroundFillIndex]=new GtkFill(Display::plateBackgroundFillIndex);

        /*
          draw->PushForeground(dynamic_cast<GtkTheme*>(OS::display->GetTheme())->helpBackgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          */

        fill[Display::graphBackgroundFillIndex]=new GtkFill(Display::graphBackgroundFillIndex);// TODO
        fill[Display::toolbarBackgroundFillIndex]=new GtkFill(Display::toolbarBackgroundFillIndex);// TODO
        fill[Display::labelBackgroundFillIndex]=new GtkFill(Display::labelBackgroundFillIndex);
        fill[Display::boxedBackgroundFillIndex]=new GtkFill(Display::boxedBackgroundFillIndex);

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        appMenuBackground=new AppMenuFill();
  #endif
#endif

        frame[Display::tabFrameIndex]=new GtkFrame(Display::tabFrameIndex);
        frame[Display::hscaleFrameIndex]=new GtkFrame(Display::hscaleFrameIndex);
        frame[Display::vscaleFrameIndex]=new GtkFrame(Display::vscaleFrameIndex);
        frame[Display::focusFrameIndex]=new GtkFrame(Display::focusFrameIndex);
        frame[Display::statuscellFrameIndex]=new GtkFrame(Display::statuscellFrameIndex);
        frame[Display::valuebarFrameIndex]=new GtkFrame(Display::valuebarFrameIndex);
        frame[Display::groupFrameIndex]=new GtkFrame(Display::groupFrameIndex);

        image[Display::hknobImageIndex]=new GtkImage(Display::hknobImageIndex);
        image[Display::vknobImageIndex]=new GtkImage(Display::vknobImageIndex);
        image[Display::arrowLeftImageIndex]=new GtkImage(Display::arrowLeftImageIndex);
        image[Display::arrowRightImageIndex]=new GtkImage(Display::arrowRightImageIndex);
        image[Display::arrowUpImageIndex]=new GtkImage(Display::arrowUpImageIndex);
        image[Display::arrowDownImageIndex]=new GtkImage(Display::arrowDownImageIndex);
        image[Display::scrollLeftImageIndex]=new GtkImage(Display::scrollLeftImageIndex);
        image[Display::scrollRightImageIndex]=new GtkImage(Display::scrollRightImageIndex);
        image[Display::scrollUpImageIndex]=new GtkImage(Display::scrollUpImageIndex);
        image[Display::scrollDownImageIndex]=new GtkImage(Display::scrollDownImageIndex);
        image[Display::comboImageIndex]=new GtkImage(Display::comboImageIndex);
        image[Display::comboEditButtonImageIndex]=new GtkImage(Display::comboEditButtonImageIndex);
        image[Display::comboDividerImageIndex]=new GtkImage(Display::comboDividerImageIndex);
        image[Display::treeExpanderImageIndex]=new GtkImage(Display::treeExpanderImageIndex);
        image[Display::hscaleKnobImageIndex]=new GtkImage(Display::hscaleKnobImageIndex);
        image[Display::vscaleKnobImageIndex]=new GtkImage(Display::vscaleKnobImageIndex);
        image[Display::leftSliderImageIndex]=new GtkImage(Display::leftSliderImageIndex);
        image[Display::rightSliderImageIndex]=new GtkImage(Display::rightSliderImageIndex);
        image[Display::topSliderImageIndex]=new GtkImage(Display::topSliderImageIndex);
        image[Display::bottomSliderImageIndex]=new GtkImage(Display::bottomSliderImageIndex);
        image[Display::menuDividerImageIndex]=new GtkImage(Display::menuDividerImageIndex,menuItem);
        image[Display::menuSubImageIndex]=new GtkImage(Display::menuSubImageIndex,menuItem);
        image[Display::menuCheckImageIndex]=new GtkImage(Display::menuCheckImageIndex);
        image[Display::menuRadioImageIndex]=new GtkImage(Display::menuRadioImageIndex);
        image[Display::hpaneHandleImageIndex]=new GtkImage(Display::hpaneHandleImageIndex);
        image[Display::vpaneHandleImageIndex]=new GtkImage(Display::vpaneHandleImageIndex);
        image[Display::radioImageIndex]=new GtkImage(Display::radioImageIndex);
        image[Display::checkImageIndex]=new GtkImage(Display::checkImageIndex);

        image[Display::ledImageIndex]=new GtkImage(Display::ledImageIndex);

        stockImage[imagePositive]=new GtkStockImage(GTK_STOCK_APPLY);
        stockImage[imageNegative]=new GtkStockImage(GTK_STOCK_CANCEL);
        stockImage[imageDefault]=new GtkStockImage(GTK_STOCK_CLOSE);
        stockImage[imageHelp]=new GtkStockImage(GTK_STOCK_HELP);
        stockImage[imageClose]=new GtkStockImage(GTK_STOCK_CLOSE);
        stockImage[imageAdd]=new GtkStockImage(GTK_STOCK_ADD);
        stockImage[imageRemove]=new GtkStockImage(GTK_STOCK_REMOVE);
        stockImage[imageClear]=new GtkStockImage(GTK_STOCK_CLEAR);
        stockImage[imageEdit]=new GtkStockImage(GTK_STOCK_EDIT);
        stockImage[imageSave]=new GtkStockImage(GTK_STOCK_SAVE);
        stockImage[imageMediaRecord]=new GtkStockImage(GTK_STOCK_MEDIA_RECORD);
        stockImage[imageMediaPlay]=new GtkStockImage(GTK_STOCK_MEDIA_PLAY);
        stockImage[imageMediaPause]=new GtkStockImage(GTK_STOCK_MEDIA_PAUSE);
        stockImage[imageMediaStop]=new GtkStockImage(GTK_STOCK_MEDIA_STOP);

#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        scrollKnobMode=scrollKnobModeNone;
  #else
        scrollKnobMode=(GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleHKnob],"has-secondary-backward-stepper") &&
                       GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleHKnob],"has-secondary-forward-stepper")) ?
                       scrollKnobModeDouble : scrollKnobModeSingle;
  #endif
#else
        scrollKnobMode=(GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleHKnob],"has-secondary-backward-stepper") &&
                       GetWidgetStyleBoolValue(gtkWidgets->widget[GtkWidgets::styleHKnob],"has-secondary-forward-stepper")) ?
                       scrollKnobModeDouble : scrollKnobModeSingle;
#endif
      }

      GtkTheme::~GtkTheme()
      {
        gtk_object_destroy(GTK_OBJECT(window));
        gtk_object_destroy(GTK_OBJECT(popup));
        gtk_object_destroy(GTK_OBJECT(menu));

        delete gtkWidgets;

        display->FreeColor(helpBackgroundColor);

        for (size_t i=0; i<Display::colorCount; i++) {
          display->FreeColor(color[i]);
        }
      }

      bool GtkTheme::IsLightTheme() const
      {
        GdkColor &bg=gtkWidgets->widget[GtkWidgets::styleWindow]->style->bg[GTK_STATE_NORMAL];

        return (unsigned long)bg.red+(unsigned long)bg.green+(unsigned long)bg.blue>3*65535/2;
      }

      bool GtkTheme::HasFontSettings() const
      {
        return false;
      }

#if GLIB_CHECK_VERSION(2,26,0)
      static std::wstring ExtractFontNameFromGSettings(const char* key)
      {
        GSettings *settings;
        gchar     *tmp;

        settings=g_settings_new("org.gnome.desktop.interface");
        tmp=g_settings_get_string(settings,key);

        if (tmp==NULL) {
          g_object_unref(settings);
          return L"";
        }

        std::wstring name(Lum::Base::StringToWString(tmp));

        if (name.empty()) {
          g_free(tmp);
          g_object_unref(settings);
          return L"";
        }

        g_free(tmp);
        g_object_unref(settings);

        size_t x=name.length()-1;

        while (x>0 && name[x]>=L'0' && name[x]<=L'9') {
          x--;
        }
        while (x>0 && name[x]==L' ') {
          x--;
        }

        return name.substr(0,x+1);
      }

      static double ExtractFontSizeFromGSettings(const char* key)
      {
        GSettings *settings;
        gchar     *tmp;

        settings=g_settings_new("org.gnome.desktop.interface");
        tmp=g_settings_get_string(settings,key);

        if (tmp==NULL) {
          g_object_unref(settings);
          return 0;
        }

        std::wstring size(Lum::Base::StringToWString(tmp));

        if (size.empty()) {
          g_free(tmp);
          g_object_unref(settings);
          return 0;
        }

        g_free(tmp);
        g_object_unref(settings);

        size_t x=size.length()-1;
        size_t value;

        while (x>0 && size[x]>=L'0' && size[x]<=L'9') {
          x--;
        }

        assert(x<size.length());

        size=size.substr(x+1);

        if (Lum::Base::WStringToNumber(size,value)) {
          return value;
        }

        assert(false);
      }
#elif defined(HAVE_LIB_GCONF)
      static std::wstring ExtractFontNameFromGConf(const char* path)
      {
        GConfClient *client;
        gchar       *tmp;

        client=gconf_client_get_default();
        tmp=gconf_client_get_string(client,path,NULL);

        if (tmp==NULL) {
          g_object_unref(client);
          return L"";
        }

        std::wstring name(Lum::Base::StringToWString(tmp));

        if (name.empty()) {
          g_free(tmp);
          g_object_unref(client);
          return L"";
        }

        g_free(tmp);
        g_object_unref(client);

        size_t x=name.length()-1;

        while (x>0 && name[x]>=L'0' && name[x]<=L'9') {
          x--;
        }
        while (x>0 && name[x]==L' ') {
          x--;
        }

        return name.substr(0,x+1);
      }

      static double ExtractFontSizeFromGConf(const char* path)
      {
        GConfClient *client;
        gchar       *tmp;

        client=gconf_client_get_default();
        tmp=gconf_client_get_string(client,path,NULL);

        if (tmp==NULL) {
          g_object_unref(client);
          return 0;
        }

        std::wstring size(Lum::Base::StringToWString(tmp));

        if (size.empty()) {
          g_free(tmp);
          g_object_unref(client);
          return 0.0;
        }

        g_free(tmp);
        g_object_unref(client);

        size_t x=size.length()-1;
        size_t value;

        while (x>0 && size[x]>=L'0' && size[x]<=L'9') {
          x--;
        }

        assert(x<size.length());

        size=size.substr(x+1);

        if (Lum::Base::WStringToNumber(size,value)) {
          return value;
        }

        assert(false);
      }
#endif

      std::wstring GtkTheme::GetProportionalFontName() const
      {
        std::wstring result;

#if defined(LUM_HAVE_LIB_HILDON)
        result=Lum::Base::StringToWString(pango_font_description_get_family(gtkWidgets->GetStyle(GtkWidgets::styleWindow)->font_desc));

        if (!result.empty()) {
          return result;
        }
#elif GLIB_CHECK_VERSION(2,26,0)
        result=ExtractFontNameFromGSettings("font-name");

        if (!result.empty()) {
          return result;
        }
#elif defined(HAVE_LIB_GCONF)
        result=ExtractFontNameFromGConf("/desktop/gnome/interface/font_name");

        if (!result.empty()) {
          return result;
        }
#endif

        result=L"Helvetica";

        return result;
      }

      double GtkTheme::GetProportionalFontSize() const
      {
        double result=0;

#if defined(LUM_HAVE_LIB_HILDON)
        result=pango_font_description_get_size(gtkWidgets->GetStyle(GtkWidgets::styleWindow)->font_desc)/PANGO_SCALE;

        if (result>=0) {
          double resolution=gdk_screen_get_resolution(gdk_screen_get_default());

          if (resolution<=0) {
            resolution=96;
          }

          return result*(resolution/72);
        }
#elif GLIB_CHECK_VERSION(2,26,0)
        result=ExtractFontSizeFromGSettings("font-name");

        if (result>=0) {
          return result;
        }
#elif defined(HAVE_LIB_GCONF)
        result=ExtractFontSizeFromGConf("/desktop/gnome/interface/font_name");

        if (result>=0) {
          double resolution=gdk_screen_get_resolution(gdk_screen_get_default());

          if (resolution<=0) {
            resolution=96;
          }

          return result*(resolution/72);
        }
#endif

        result=9;

        double resolution=gdk_screen_get_resolution(gdk_screen_get_default());

        if (resolution<=0) {
          resolution=96;
        }

        return result*(resolution/72);
      }

      std::wstring GtkTheme::GetFixedFontName() const
      {
        std::wstring result;

#if GLIB_CHECK_VERSION(2,26,0)
        result=ExtractFontNameFromGSettings("monospace-font-name");

        if (!result.empty()) {
          return result;
        }

#elif defined(HAVE_LIB_GCONF)
        result=ExtractFontNameFromGConf("/desktop/gnome/interface/monospace_font_name");

        if (!result.empty()) {
          return result;
        }
#endif

        result=L"Fixed";

        return result;
      }

      double GtkTheme::GetFixedFontSize() const
      {
        double result=0;

#if GLIB_CHECK_VERSION(2,26,0)
        result=ExtractFontSizeFromGSettings("monospace-font-name");

        if (result>=0) {
          return result;
        }
#elif defined(HAVE_LIB_GCONF)
        result=ExtractFontSizeFromGConf("/desktop/gnome/interface/monospace_font_name");
        if (result>=0) {
          double resolution=gdk_screen_get_resolution(gdk_screen_get_default());

          if (resolution<=0) {
            resolution=96;
          }

          return result*(resolution/72);
        }
#endif

        result=9;

        double resolution=gdk_screen_get_resolution(gdk_screen_get_default());

        if (resolution<=0) {
          resolution=96;
        }

        return result*(resolution/72);
      }

      size_t GtkTheme::GetSpaceHorizontal(Display::Space space) const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        switch (space) {
        case Display::spaceWindowBorder:
          return 0;
        case Display::spaceInterGroup:
          return 12;
        case Display::spaceGroupIndent:
          return 8;
        case Display::spaceInterObject:
          return 4;
        case Display::spaceObjectBorder:
          return 2;
        case Display::spaceIntraObject:
          return 4;
        case Display::spaceLabelObject:
          return 8;
        case Display::spaceObjectDetail:
          return 12;
        }
#else
        switch (space) {
        case Display::spaceWindowBorder:
          return 12;
        case Display::spaceInterGroup:
          return 18;
        case Display::spaceGroupIndent:
          return 12;
        case Display::spaceInterObject:
          return 6;
        case Display::spaceObjectBorder:
          return 6;
        case Display::spaceIntraObject:
          return 6;
        case Display::spaceLabelObject:
          return 12;
        case Display::spaceObjectDetail:
          return 16;
        }

#endif
        return 1;
      }

      size_t GtkTheme::GetSpaceVertical(Display::Space space) const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        switch (space) {
        case Display::spaceWindowBorder:
          return 0;
        case Display::spaceInterGroup:
          return 12;
        case Display::spaceGroupIndent:
          return 18;
        case Display::spaceInterObject:
          return 4;
        case Display::spaceObjectBorder:
          return 2;
        case Display::spaceIntraObject:
          return 4;
        case Display::spaceLabelObject:
          return 4;
        case Display::spaceObjectDetail:
          return 12;
        }
#else
        switch (space) {
        case Display::spaceWindowBorder:
          return 12;
        case Display::spaceInterGroup:
          return 18;
        case Display::spaceGroupIndent:
          return 12;
        case Display::spaceInterObject:
          return 6;
        case Display::spaceObjectBorder:
          return 3;
        case Display::spaceIntraObject:
          return 6;
        case Display::spaceLabelObject:
          return 6;
        case Display::spaceObjectDetail:
          return 16;
        }

#endif
        return 1;
      }

      size_t GtkTheme::GetIconWidth() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,2,0)
        return hildon_get_icon_pixel_size(HILDON_ICON_SIZE_FINGER);
  #else
        return 16;
  #endif
#else
        return 16;
#endif
      }

      size_t GtkTheme::GetIconHeight() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,2,0)
        return hildon_get_icon_pixel_size(HILDON_ICON_SIZE_FINGER);
  #else
        return 16;
  #endif
#else
        return 16;
#endif
      }

      size_t GtkTheme::GetDragStartSensitivity() const
      {
        return 11;
      }

      size_t GtkTheme::GetMouseClickHoldSensitivity() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        return 5;
#else
        return 10;
#endif
      }

      unsigned long GtkTheme::GetCursorBlinkTimeMicroSeconds() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        return 0;
#else
        return Base::Theme::GetCursorBlinkTimeMicroSeconds();
#endif
      }

      size_t GtkTheme::GetMinimalButtonWidth() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        return 174;
  #else
        return Lum::Base::GetSize(Lum::Base::Size::stdCharWidth,8);
  #endif
#else
        return Lum::Base::GetSize(Lum::Base::Size::stdCharWidth,8);
#endif
      }

      size_t GtkTheme::GetFirstTabOffset() const
      {
        return 0;
      }

      size_t GtkTheme::GetLastTabOffset() const
      {
        return OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
      }

      size_t GtkTheme::GetMinimalTableRowHeight() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        return 70;
  #else
        return 0;
  #endif
#else
        return 0;
#endif
      }

      size_t GtkTheme::GetHorizontalFuelBarMinWidth() const
      {
        return GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleProgressH],
                                      "min-horizontal-bar-width");
      }

      size_t GtkTheme::GetHorizontalFuelBarMinHeight() const
      {
        return GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleProgressH],
                                      "min-horizontal-bar-height");
      }

      size_t GtkTheme::GetVerticalFuelBarMinWidth() const
      {
        return GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleProgressV],
                                      "min-vertical-bar-width");
      }

      size_t GtkTheme::GetVerticalFuelBarMinHeight() const
      {
        return GetWidgetStyleIntValue(gtkWidgets->widget[GtkWidgets::styleProgressV],
                                      "min-vertical-bar-height");
      }

      OS::Theme::ScrollKnobMode GtkTheme::GetScrollKnobMode() const
      {
        return scrollKnobMode;
      }

      OS::Theme::ToolbarPosition GtkTheme::GetToolbarPosition() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
       return OS::Theme::toolbarBottom;
#else
       return OS::Theme::toolbarTop;
#endif
      }

      bool GtkTheme::ShowButtonImages() const
      {
        return GetSettingsBoolValue("gtk-button-images");
      }

      bool GtkTheme::RequestFingerFriendlyControls() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        return true;
  #else
        return false;
  #endif
#else
        return false;
#endif
      }

      Lum::Base::Size GtkTheme::GetFingerFriendlyMinWidth() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        // DPI settings for hildon are currently wrong and lead
        // to wrong mm measurements
        return Lum::Base::Size(Lum::Base::Size::pixel,70);
#else
        return Lum::OS::Base::Theme::GetFingerFriendlyMinWidth();
#endif
      }

      Lum::Base::Size GtkTheme::GetFingerFriendlyMinHeight() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        // DPI settings for hildon are currently wrong and lead
        // to wrong mm measurements
        return Lum::Base::Size(Lum::Base::Size::pixel,70);
#else
        return Lum::OS::Base::Theme::GetFingerFriendlyMinHeight();
#endif
      }

      bool GtkTheme::ShowKeyShortcuts() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        return false;
#else
        return true;
#endif
      }

      bool GtkTheme::FocusOnEditableControlsOnly() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        return true;
#else
        return false;
#endif
      }

      OS::Theme::MenuType GtkTheme::GetMenuType() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        return menuTypeCustom;
#else
        return menuTypeStrip;
#endif
      }

      int GtkTheme::GetSubMenuHorizontalOffset() const
      {
        int horizOffset=0;

        gtk_widget_style_get(GTK_WIDGET(menu),
                             "horizontal-offset",&horizOffset,NULL);

        return horizOffset;

      }

      bool GtkTheme::OptimizeMenus() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
        return true;
#else
        return false;
#endif
      }

      bool GtkTheme::SubdialogsAlwaysFlexHoriz() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        return true;
  #else
        return OS::Base::Theme::SubdialogsAlwaysFlexHoriz();
  #endif
#else
        return OS::Base::Theme::SubdialogsAlwaysFlexHoriz();
#endif
      }

      bool GtkTheme::PopupsAreDialogs() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        return true;
  #else
        return false;
  #endif
#else
        return false;
#endif
      }

      Theme::DialogActionPosition GtkTheme::GetDialogActionPosition() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        if (display->IsLandscape()) {
          return actionPosVertButtonRowRight;
        }
        else {
          return actionPosHorizButtonRowBelow;
        }
  #else
        return actionPosHorizButtonRowBelow;
  #endif
#else
        return actionPosHorizButtonRowBelow;
#endif
      }

      bool GtkTheme::ImplicitCancelAction() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        return true;
  #else
        return false;
  #endif
#else
        return false;
#endif
      }

      bool GtkTheme::ImplicitDefaultAction() const
      {
#if defined(LUM_HAVE_LIB_HILDON)
  #if HILDON_CHECK_VERSION(2,1,0)
        return true;
  #else
        return false;
  #endif
#else
        return false;
#endif
      }
    }
  }
}
