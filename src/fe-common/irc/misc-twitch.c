#include <string.h>
#include <glib.h>

/**
 * Decode Twitch IRC escape sequences into a newly allocated string.
 * Handles: \s -> space, \\ -> \, \: -> ;, \r, \n
 */
static char *twitch_unescape_to_gstr(const char *src, size_t len)
{
    GString *out = g_string_sized_new(len);
    for (size_t i = 0; i < len; i++) {
        if (src[i] == '\\' && i + 1 < len) {
            i++;
            switch (src[i]) {
                case 's': g_string_append_c(out, ' '); break;
                case '\\': g_string_append_c(out, '\\'); break;
                case ':': g_string_append_c(out, ';'); break;
                case 'r': g_string_append_c(out, '\r'); break;
                case 'n': g_string_append_c(out, '\n'); break;
                default:  g_string_append_c(out, src[i]); break;
            }
        } else {
            g_string_append_c(out, src[i]);
        }
    }
    return g_string_free(out, FALSE); // returns newly allocated NUL-terminated string
}

/**
 * Parse Twitch tag string and extract "login" and "system-msg".
 *
 * Example input:
 *   "badge-info=subscriber/6;badges=subscriber/6;login=myUser;system-msg=User\\sjoined\\sthe\\schannel"
 *
 * Output:
 *   *login_out = g_strdup("myUser")
 *   *system_msg_out = g_strdup("User joined the channel")
 *
 * Caller must g_free() both if non-NULL.
 */
void twitch_extract_login_and_systemmsg(const char *tags_str,
                                        char **login_out,
                                        char **system_msg_out)
{
    const char *ptr, *end, *eq, *sep, *key_start, *key_end, *val_start, *val_end;
    size_t key_len, val_len;

    if (login_out) *login_out = NULL;
    if (system_msg_out) *system_msg_out = NULL;

    if (!tags_str || !*tags_str)
        return;

    ptr = tags_str;
    end = tags_str + strlen(tags_str);

    while (ptr < end) {
        eq = memchr(ptr, '=', end - ptr);
        sep = memchr(ptr, ';', end - ptr);
        if (!sep) sep = end;

        key_start = ptr;
        key_end = (eq && eq < sep) ? eq : sep;
        key_len = key_end - key_start;

        val_start = (eq && eq < sep) ? eq + 1 : NULL;
        val_end = sep;
        val_len = (val_start) ? (val_end - val_start) : 0;

        if (key_len == 5 && strncmp(key_start, "login", 5) == 0 &&
            val_start && login_out) {
            *login_out = g_strndup(val_start, val_len);
        }
        else if (key_len == 10 && strncmp(key_start, "system-msg", 10) == 0 &&
                 val_start && system_msg_out) {
            *system_msg_out = twitch_unescape_to_gstr(val_start, val_len);
        }

        if (sep >= end)
            break;
        ptr = sep + 1;
    }
}
