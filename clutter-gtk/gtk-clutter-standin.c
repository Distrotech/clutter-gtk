/* gtk-clutter-standin.c: a widget that stands-in for a ClutterActor
 *
 * Copyright (C) 2009 Collabora Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not see <http://www.fsf.org/licensing>.
 *
 * Authors:
 *   Danielle Madeley  <danielle.madeley@collabora.co.uk>
 */

/**
 * SECTION:gtk-clutter-standin
 * @short_description: a Widget which is a standin for a ClutterActor
 *
 * Since: 1.0
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-standin-private.h"
#include "gtk-clutter-offscreen.h"
#include "gtk-clutter-util.h"

#include <glib-object.h>
#include <math.h>

G_DEFINE_TYPE (GtkClutterStandin, gtk_clutter_standin, GTK_TYPE_WIDGET);

#define GTK_CLUTTER_STANDIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_CLUTTER_TYPE_STANDIN, GtkClutterStandinPrivate))

enum
{

  PROP_0,
  PROP_ACTOR
};

struct _GtkClutterStandinPrivate
{
  ClutterActor *bin;
  ClutterActor *actor;

  guint actor_on_stage : 1;
};

static void
gtk_clutter_standin_put_actor_on_stage (GtkClutterStandin *self)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (self)->priv;
  GtkWidget *parent;

  if (priv->actor_on_stage)
    return;

  /* find the stage that this stand-in is placed upon and place our actor
   * upon it */
  parent = GTK_WIDGET (self);

  while ((parent = gtk_widget_get_parent (parent)))
    {
      if (GTK_CLUTTER_IS_OFFSCREEN (parent))
        break;
    }

  if (parent == NULL)
    return;

  g_return_if_fail (GTK_CLUTTER_IS_OFFSCREEN (parent));

  /* GtkClutterActor can accept a GtkClutterStandin as a child */
  clutter_container_add_actor (CLUTTER_CONTAINER (GTK_CLUTTER_OFFSCREEN (parent)->actor),
                               priv->bin);
  priv->actor_on_stage = TRUE;
}

static void
gtk_clutter_standin_send_configure (GtkClutterStandin *self)
{
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new (GDK_CONFIGURE);

  widget = GTK_WIDGET (self);

  event->configure.window = g_object_ref (widget->window);
  event->configure.send_event = TRUE;
  event->configure.x = widget->allocation.x;
  event->configure.y = widget->allocation.y;
  event->configure.width = widget->allocation.width;
  event->configure.height = widget->allocation.height;

  gtk_widget_event (widget, event);
  gdk_event_free (event);
}

static void
gtk_clutter_standin_get_property (GObject    *self,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  switch (property_id)
    {
    case PROP_ACTOR:
      g_value_set_object (value, gtk_clutter_standin_get_actor (GTK_CLUTTER_STANDIN (self)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
      break;
    }
}

static void
gtk_clutter_standin_set_property (GObject      *self,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  switch (property_id)
    {
    case PROP_ACTOR:
      gtk_clutter_standin_set_actor (GTK_CLUTTER_STANDIN (self), g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
      break;
    }
}

static void
gtk_clutter_standin_dispose (GObject *gobject)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (gobject)->priv;

  if (priv->bin != NULL)
    {
      gtk_clutter_standin_set_actor (GTK_CLUTTER_STANDIN (gobject), NULL);
      clutter_actor_destroy (priv->bin);
      priv->bin = NULL;
    }

  G_OBJECT_CLASS (gtk_clutter_standin_parent_class)->dispose (gobject);
}

static void
gtk_clutter_standin_show (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->show (widget);

  clutter_actor_show (priv->bin);
}

static void
gtk_clutter_standin_show_all (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->show_all (widget);

  clutter_actor_show_all (priv->bin);
}

static void
gtk_clutter_standin_hide (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  /* gtk emits a hide signal during dispose, so it's possible we may
   * have already disposed priv->stage. */
  if (priv->bin != NULL)
    clutter_actor_hide (priv->bin);

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->hide (widget);
}

static void
gtk_clutter_standin_hide_all (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  /* gtk emits a hide signal during dispose, so it's possible we may
   * have already disposed priv->stage. */
  if (priv->bin != NULL)
    clutter_actor_hide_all (priv->bin);

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->hide_all (widget);
}

static void
gtk_clutter_standin_realize (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;
  GdkWindowAttr attributes;
  int attributes_mask;

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);

  attributes.event_mask = gtk_widget_get_events (widget)
                        | GDK_EXPOSURE_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                   &attributes,
                                   attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

  clutter_actor_realize (priv->bin);

  if (GTK_WIDGET_VISIBLE (widget))
    clutter_actor_show (priv->bin);

  gtk_clutter_standin_send_configure (GTK_CLUTTER_STANDIN (widget));
}

static void
gtk_clutter_standin_unrealize (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  clutter_actor_hide (priv->bin);

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->unrealize (widget);
}

static void
gtk_clutter_standin_size_request (GtkWidget      *self,
                                  GtkRequisition *requisition)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (self)->priv;

  _gtk_clutter_standin_bin_gtk_size_request (GTK_CLUTTER_STANDIN_BIN (priv->bin), requisition);
}

static void
gtk_clutter_standin_size_allocate (GtkWidget     *widget,
                                   GtkAllocation *allocation)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;
  GtkClutterStandinBin *bin = GTK_CLUTTER_STANDIN_BIN (priv->bin);
  GtkAllocation root;

  widget->allocation = *allocation;

  if (GTK_WIDGET_REALIZED (widget))
    {
      gdk_window_move_resize (widget->window,
                              allocation->x, allocation->y,
                              allocation->width, allocation->height);

      gtk_clutter_standin_send_configure (GTK_CLUTTER_STANDIN (widget));
    }

  gtk_clutter_calculate_actor_allocation (widget, &root);
  _gtk_clutter_standin_bin_gtk_size_allocate (bin, &root);
}

static void
gtk_clutter_standin_map (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  gtk_clutter_standin_put_actor_on_stage (GTK_CLUTTER_STANDIN (widget));
  clutter_actor_map (priv->bin);

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->map (widget);
}

static void
gtk_clutter_standin_unmap (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  clutter_actor_unmap (priv->bin);

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->unmap (widget);
}

static void
gtk_clutter_standin_parent_set (GtkWidget *self,
                                GtkWidget *old_parent)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (self)->priv;

  priv->actor_on_stage = FALSE;
  gtk_clutter_standin_put_actor_on_stage (GTK_CLUTTER_STANDIN (self));
}

static void
gtk_clutter_standin_class_init (GtkClutterStandinClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GParamSpec *pspec;

  g_type_class_add_private (klass, sizeof (GtkClutterStandinPrivate));

  gobject_class->get_property = gtk_clutter_standin_get_property;
  gobject_class->set_property = gtk_clutter_standin_set_property;
  gobject_class->dispose = gtk_clutter_standin_dispose;

  widget_class->size_request = gtk_clutter_standin_size_request;
  widget_class->size_allocate = gtk_clutter_standin_size_allocate;
  widget_class->realize = gtk_clutter_standin_realize;
  widget_class->unrealize = gtk_clutter_standin_unrealize;
  widget_class->show = gtk_clutter_standin_show;
  widget_class->show_all = gtk_clutter_standin_show_all;
  widget_class->hide = gtk_clutter_standin_hide;
  widget_class->hide_all = gtk_clutter_standin_hide_all;
  widget_class->map = gtk_clutter_standin_map;
  widget_class->unmap = gtk_clutter_standin_unmap;
  widget_class->parent_set = gtk_clutter_standin_parent_set;

  pspec = g_param_spec_object ("actor",
                               "Actor",
                               "The Clutter actor this widget is standing in for",
                               CLUTTER_TYPE_ACTOR,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_ACTOR, pspec);
}

static void
gtk_clutter_standin_bin_destroyed (GtkClutterStandin *self,
                                   ClutterActor      *bin)
{
  /* our bin has been destroyed, set the pointer to NULL and schedule
   * ourselves for destruction too */
  GTK_CLUTTER_STANDIN (self)->priv->bin = NULL;

  gtk_widget_destroy (GTK_WIDGET (self));
}

static void
gtk_clutter_standin_init (GtkClutterStandin *self)
{
  GtkClutterStandinPrivate *priv;

  self->priv = priv = GTK_CLUTTER_STANDIN_GET_PRIVATE (self);

  priv->bin = g_object_new (GTK_CLUTTER_TYPE_STANDIN_BIN, NULL);
  GTK_CLUTTER_STANDIN_BIN (priv->bin)->standin = GTK_WIDGET (self);
  clutter_actor_hide (priv->bin);

  g_signal_connect_swapped (priv->bin, "destroy",
                            G_CALLBACK (gtk_clutter_standin_bin_destroyed),
                            self);
}

/**
 * gtk_clutter_standin_get_actor:
 * @self: a #GtkClutterStandin
 *
 * Retrieves a pointer to the actor that is represented by the standin widget
 *
 * Return value: (transfer none): a #ClutterActor or %NULL
 *
 * Since: 1.0
 */
ClutterActor *
gtk_clutter_standin_get_actor (GtkClutterStandin *self)
{
  g_return_val_if_fail (GTK_CLUTTER_IS_STANDIN (self), NULL);

  return GTK_CLUTTER_STANDIN_BIN (self->priv->bin)->child;
}

/**
 * gtk_clutter_standin_set_actor:
 * @self: the #GtkClutterStandin
 * @actor: (allow-none): a #ClutterActor to stand in for, or %NULL
 *
 * Sets the actor for which the #GtkClutterStandin stands for
 *
 * Since: 1.0
 */
void
gtk_clutter_standin_set_actor (GtkClutterStandin *self,
                               ClutterActor      *actor)
{
  GtkClutterStandinPrivate *priv;
  ClutterActor *old_actor;

  g_return_if_fail (GTK_CLUTTER_IS_STANDIN (self));
  g_return_if_fail (actor == NULL || CLUTTER_IS_ACTOR (actor));
  g_return_if_fail (actor == NULL || clutter_actor_get_parent (actor) == NULL);

  priv = GTK_CLUTTER_STANDIN (self)->priv;

  old_actor = gtk_clutter_standin_get_actor (self);
  if (old_actor != NULL)
    clutter_container_remove_actor (CLUTTER_CONTAINER (priv->bin), old_actor);

  if (actor != NULL)
    {
      /* ensure the actor is hidden; this ensures it won't be shown when
       * we parent it */
      clutter_actor_hide (actor);
      clutter_container_add_actor (CLUTTER_CONTAINER (priv->bin), actor);
    }
}

/**
 * gtk_clutter_standin_new:
 * @actor: the #ClutterActor to stand-in for (or NULL)
 *
 * Creates a new #GtkClutterStandin widget. This widget is used as a stand-in
 * in the GTK+ widget tree for a widget that is sitting as a separate actor
 * on the #ClutterStage this widget is sat on.
 *
 * This requires the widget tree to be embedded within a #GtkClutterActor.
 *
 * Return value: the newly created #GtkClutterStandin
 *
 * Since: 1.0
 */
GtkWidget *
gtk_clutter_standin_new (ClutterActor *actor)
{
  g_return_val_if_fail (actor == NULL || CLUTTER_IS_ACTOR (actor), NULL);

  return g_object_new (GTK_CLUTTER_TYPE_STANDIN,
                       "actor", actor,
                       NULL);
}