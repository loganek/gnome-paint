#ifndef GP_ISELECTION_TOOL_H_
#define GP_ISELECTION_TOOL_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define GP_TYPE_ISELECTION_TOOL gp_iselection_tool_get_type ()
G_DECLARE_INTERFACE (GPISelectionTool, gp_iselection_tool, GP, ISELECTION_TOOL, GObject)

struct _GPISelectionToolInterface
{
    GTypeInterface parent_iface;

    void (*init_from_params) (
                GPISelectionTool *iselection_tool,
                gpointer         selection_params);

    void (*destroy_params) (
                GPISelectionTool *selection_tool,
                gpointer          selection_params);
};

void gp_iselection_tool_init_from_params (
        GPISelectionTool *iselection_tool,
        gpointer          selection_params);

void gp_iselection_tool_destroy_params (
        GPISelectionTool *iselection_tool,
        gpointer          selection_params);

G_END_DECLS

#endif // GP_ISELECTION_TOOL_H_
