/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2018  <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-testfilter
 *
 * FIXME:Describe testfilter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch --gst-plugin-path=./src/.libs -v -m fakesrc ! testfilter silent=TRUE ! fakesink
 * ]|
 * </refsect2>
 */

/// Debug
/// https://gstreamer.freedesktop.org/documentation/plugin-development/basics/testapp.html
/// https://gstreamer.freedesktop.org/documentation/tutorials/basic/debugging-tools.html

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gsttestfilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_test_filter_debug);
#define GST_CAT_DEFAULT gst_test_filter_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

/* properties */
enum
{
  PROP_0,
  PROP_SILENT
  /* FILL ME */
};

/// Pad
/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_test_filter_parent_class parent_class
/// define Element class.
/// based on GstElementClass(GST_TYPE_ELEMENT) class.
G_DEFINE_TYPE (GstTestFilter, gst_test_filter, GST_TYPE_ELEMENT);
/// parent_class global variable is declared.

static void gst_test_filter_finalize (GObject * object);

static void gst_test_filter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_test_filter_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstStateChangeReturn gst_test_filter_change_state (GstElement * element, GstStateChange transition);

static gboolean gst_test_filter_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static gboolean gst_test_filter_sink_query (GstPad * pad, GstObject * parent, GstQuery * query);
static GstFlowReturn gst_test_filter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);
static gboolean gst_test_filter_src_query (GstPad * pad, GstObject * parent, GstQuery * query);

/* GObject vmethod implementations */

/// initialize Element class.
/* initialize the testfilter's class */
static void
gst_test_filter_class_init (GstTestFilterClass * klass)
{
  GST_LOG("%" GST_PTR_FORMAT, klass);

  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  /* define virtual function pointers */
  gobject_class->finalize = gst_test_filter_finalize;
  /// Property
  gobject_class->set_property = gst_test_filter_set_property;
  gobject_class->get_property = gst_test_filter_get_property;

  /* define properties */
  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  /// set Element details.
  gst_element_class_set_details_simple(gstelement_class,
    "TestFilter",
    "FIXME:Generic",
    "FIXME:Generic Template Element",
    " <<user@hostname.org>>");

  /// set State handler.
  gstelement_class->change_state = gst_test_filter_change_state;

  /// register Source Pad.
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  /// register Sink Pad.
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/// initialize Element.
/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_test_filter_init (GstTestFilter * filter)
{
  GST_LOG_OBJECT(filter, " ");

  /// create Sink Pad.
  /* pad through which data comes in to the element */
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  /* pads are configured here with gst_pad_set_*_function () */
  /// set Event function.
  gst_pad_set_event_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_test_filter_sink_event));
  /// set Query function.
  gst_pad_set_query_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_test_filter_sink_query));
  /// set Chain function.
  /* configure chain function on the pad before adding the pad to the element */
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_test_filter_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  GST_PAD_SET_PROXY_ALLOCATION (filter->sinkpad);
  /// add Sink Pad.
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  /// create Source Pad.
  /* pad through which data goes out of the element */
  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  /* pads are configured here with gst_pad_set_*_function () */
  /// set Query function.
  gst_pad_set_query_function (filter->srcpad,
                              GST_DEBUG_FUNCPTR(gst_test_filter_src_query));
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  /// add Source Pad.
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  /* properties initial value */
  filter->silent = FALSE;
}

static void
gst_test_filter_finalize (GObject * object)
{
  GstTestFilter *filter = GST_TESTFILTER (object);

  GST_LOG_OBJECT(filter, " ");

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_test_filter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstTestFilter *filter = GST_TESTFILTER (object);

  GST_LOG_OBJECT(filter, "%d", prop_id);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_test_filter_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstTestFilter *filter = GST_TESTFILTER (object);

  GST_LOG_OBJECT(filter, "%d", prop_id);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstStateChangeReturn
gst_test_filter_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstTestFilter *filter = GST_TESTFILTER (element);

#if 0
  GST_LOG_OBJECT(filter, "change: %s", gst_state_change_get_name(transition));
#else
  GST_LOG_OBJECT(filter, "change: %s->%s",
      gst_element_state_get_name(GST_STATE_TRANSITION_CURRENT(transition)),
      gst_element_state_get_name(GST_STATE_TRANSITION_NEXT(transition)));
#endif

  /// Upwards(NULL->READY->PAUSED->PLAYING)
  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
    case GST_STATE_CHANGE_READY_TO_PAUSED:
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      break;
    case GST_STATE_CHANGE_READY_TO_READY:
    case GST_STATE_CHANGE_PAUSED_TO_PAUSED:
    case GST_STATE_CHANGE_PLAYING_TO_PLAYING:
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  /// Downwards(NULL<-READY<-PAUSED<-PLAYING)
  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
    case GST_STATE_CHANGE_PAUSED_TO_READY:
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PAUSED:
    case GST_STATE_CHANGE_READY_TO_READY:
    case GST_STATE_CHANGE_NULL_TO_NULL:
      break;
    default:
      break;
  }

  return ret;
}


/* GstElement vmethod implementations */

/// Event function
/// Event(Control)
/* this function handles sink events */
static gboolean
gst_test_filter_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstTestFilter *filter;
  gboolean ret;

  filter = GST_TESTFILTER (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  /// GstEventType
  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    /* we should handle the format here */
    {
      GstCaps * caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    case GST_EVENT_EOS:
    /* end-of-stream, we should close down all stream leftovers here */
    default:
      /* just call the default handler */
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

/// Query function (for sink)
static gboolean
gst_test_filter_sink_query (GstPad * pad, GstObject * parent, GstQuery * query)
{
  GstTestFilter *filter;
  gboolean ret = FALSE;

  filter = GST_TESTFILTER (parent);

  GST_LOG_OBJECT(filter, "%s:%s", GST_DEBUG_PAD_NAME(pad));

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_ALLOCATION:
    default:
      /* just call the default handler */
      ret = gst_pad_query_default (pad, parent, query);
      break;
  }
  return ret;
}

/// Chain function
/// Buffer(Content)
/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_test_filter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstTestFilter *filter;

  filter = GST_TESTFILTER (parent);

  GST_LOG_OBJECT(filter, "%s:%s", GST_DEBUG_PAD_NAME(pad));

  if (filter->silent == FALSE) {
    /// GstBuffer
    g_print("Have data of size %" G_GSIZE_FORMAT " bytes!\n", gst_buffer_get_size(buf));
    g_print("Buffer writable is %s.\n", gst_buffer_is_writable(buf) ? "TRUE" : "FALSE");

    if (buf->pool) {
      /// GstBufferPool
      g_print("buffer belongs to GstBufferPool.\n");
    }

    {
      /// GstMapInfo from GstBuffer.
      GstMapInfo info;
      gboolean result = gst_buffer_map(buf, &info, GST_MAP_READ);
      if (result) {
        g_print("read from GstBuffer: %c.\n", *info.data);
        gst_buffer_unmap(buf, &info);
      }
    }

    const guint n_memory = gst_buffer_n_memory(buf);
    g_print("Have %u memory(max=%d).\n", n_memory, gst_buffer_get_max_memory());
    if (n_memory > 0) {
      /// GstMemory
      GstMemory* mem = gst_buffer_get_memory(buf, 0);
      gsize offset = 0;
      gsize maxsize = 0;
      gsize size = gst_memory_get_sizes(mem, &offset, &maxsize);
      g_print("Memory(%u) size %" G_GSIZE_FORMAT " offset %" G_GSIZE_FORMAT " maxsize %" G_GSIZE_FORMAT ".\n", 0, size, offset, maxsize);

      /// GstMapInfo from GstMemory.
      GstMapInfo info;
      gboolean result = gst_memory_map(mem, &info, GST_MAP_READ);
      if (result) {
        g_print("read from GstMemory: %c.\n", *info.data);
        gst_memory_unmap(mem, &info);
      }

      gst_memory_unref(mem);
    }
  }

  /* just push out the incoming buffer without touching it */
  return gst_pad_push (filter->srcpad, buf);
}

/// Query function (for source)
static gboolean
gst_test_filter_src_query (GstPad * pad, GstObject * parent, GstQuery * query)
{
  GstTestFilter *filter;
  gboolean ret = FALSE;

  filter = GST_TESTFILTER (parent);

  GST_LOG_OBJECT(filter, "%s:%s", GST_DEBUG_PAD_NAME(pad));

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_POSITION:
      /* we should report the current position */
      break;
    case GST_QUERY_DURATION:
      /* we should report the duration here */
      break;
    case GST_QUERY_CAPS:
      /* we should report the supported caps here */
      break;
    default:
      /* just call the default handler */
      ret = gst_pad_query_default (pad, parent, query);
      break;
  }
  return ret;
}


/// initialize Plugin.
/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
testfilter_init (GstPlugin * testfilter)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template testfilter' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_test_filter_debug, "testfilter",
      0, "Template testfilter");

  /// register Element.
  return gst_element_register (testfilter, "testfilter", GST_RANK_NONE,
      GST_TYPE_TESTFILTER);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstplugin"
#endif

/// define Plugin.
/// set Plugin informations.
/* gstreamer looks for this structure to register testfilters
 *
 * exchange the string 'Template testfilter' with your testfilter description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    testfilter,
    "Template testfilter",
    testfilter_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
