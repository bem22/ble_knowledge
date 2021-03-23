//
// Created by bem22 on 10/03/2021.
//

#ifndef SNIPPETS_PANORAMALABS_AGENT_H
#define SNIPPETS_PANORAMALABS_AGENT_H


typedef enum {
    FILTER_NO_FILTER = 0,
    FILTER_WHITELIST = 1,
    FILTER_SINGLE_DEVICE = 2,
} PanoramaAgentFilterFlags;

extern int agentFilterFlag;

char **whitelisted;

void set_trusted(gchar *path);

int on_request_confirmation(PanoramaOrgBluezAgent1 *agent,
                            GDBusMethodInvocation *invocation,
                            gchar *path,
                            guint32 passkey,
                            gpointer userdata);

int on_request_authorization(PanoramaOrgBluezAgent1 *agent,
                             GDBusMethodInvocation *invocation,
                             gchar *path);

int set_agent_filter(PanoramaAgentFilterFlags flag);

int autopair_init ();
#endif //SNIPPETS_PANORAMALABS_AGENT_H
