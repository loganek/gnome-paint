#include "gp-iselectiontool.h"

G_DEFINE_INTERFACE (GPISelectionTool, gp_iselection_tool, G_TYPE_OBJECT)

static void
gp_iselection_tool_default_init (GPISelectionToolInterface *iface)
{
}

void
gp_iselection_tool_init_from_params (
        GPISelectionTool *iselection_tool,
        gpointer          selection_params)
{
    GPISelectionToolInterface *iface;

    g_return_if_fail (GP_IS_ISELECTION_TOOL (iselection_tool));

    iface = GP_ISELECTION_TOOL_GET_IFACE (iselection_tool);

    iface->init_from_params (iselection_tool, selection_params);
}

void
gp_iselection_tool_destroy_params (
        GPISelectionTool *iselection_tool,
        gpointer          selection_params)
{
    GPISelectionToolInterface *iface;

    g_return_if_fail (GP_IS_ISELECTION_TOOL (iselection_tool));

    iface = GP_ISELECTION_TOOL_GET_IFACE (iselection_tool);

    iface->destroy_params (iselection_tool, selection_params);
}
