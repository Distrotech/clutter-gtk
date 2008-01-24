/* gtk-clutter-embed.h: Embeddable ClutterStage
 *
 * Copyright (C) 2007 OpenedHand
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
 *   Iain Holmes  <iain@openedhand.com>
 *   Emmanuele Bassi  <ebassi@openedhand.com>
 */

#ifndef __GTK_CLUTTER_EMBED_H__
#define __GTK_CLUTTER_EMBED_H__

#include <gtk/gtkwidget.h>
#include <clutter/clutter-actor.h>

G_BEGIN_DECLS

#define GTK_TYPE_CLUTTER_EMBED          (gtk_clutter_embed_get_type ())
#define GTK_CLUTTER_EMBED(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), GTK_TYPE_CLUTTER_EMBED, GtkClutterEmbed))
#define GTK_IS_CLUTTER_EMBED(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTK_TYPE_CLUTTER_EMBED))
#define GTK_CLUTTER_EMBED_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST ((k), GTK_TYPE_CLUTTER_EMBED, GtkClutterEmbedClass))
#define GTK_IS_CLUTTER_EMBED_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), GTK_TYPE_CLUTTER_EMBED))
#define GTK_CLUTTER_EMBED_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), GTK_TYPE_CLUTTER_EMBED, GtkClutterEmbedClass))

typedef struct _GtkClutterEmbed         GtkClutterEmbed;
typedef struct _GtkClutterEmbedPrivate  GtkClutterEmbedPrivate;
typedef struct _GtkClutterEmbedClass    GtkClutterEmbedClass;

/**
 * GtkClutterEmbed:
 *
 * A #GtkWidget containing the default Clutter stage.
 *
 * Since: 0.6
 */
struct _GtkClutterEmbed
{
  /*< private >*/
  GtkWidget parent_instance;

  GtkClutterEmbedPrivate *priv;
};

/**
 * GtkClutterEmbedClass:
 *
 * Base class for #GtkClutterEmbed.
 *
 * Since: 0.6
 */
struct _GtkClutterEmbedClass
{
  /*< private >*/
  GtkWidgetClass parent_class;

  /* padding for future expansion */
  void (*_clutter_gtk_reserved1) (void);
  void (*_clutter_gtk_reserved2) (void);
  void (*_clutter_gtk_reserved3) (void);
  void (*_clutter_gtk_reserved4) (void);
  void (*_clutter_gtk_reserved5) (void);
  void (*_clutter_gtk_reserved6) (void);
};

GType         gtk_clutter_embed_get_type  (void) G_GNUC_CONST;
GtkWidget *   gtk_clutter_embed_new       (void);
ClutterActor *gtk_clutter_embed_get_stage (GtkClutterEmbed *embed);

G_END_DECLS

#endif /* __GTK_CLUTTER_EMBED_H__ */