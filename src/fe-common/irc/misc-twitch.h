#ifndef IRSSI_FE_COMMON_IRC_MISC_TWITCH_H
#define IRSSI_FE_COMMON_IRC_MISC_TWITCH_H

void twitch_extract_login_and_systemmsg(const char *raw,
                                        char **login_out,
                                        char **system_msg_out);

#endif
