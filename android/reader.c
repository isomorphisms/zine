/* NativeActivity boundary. Android owns text shaping, scrolling, accessibility
 * and source-link opening. No authored Java or DEX is needed for this reader. */
#include <android/asset_manager.h>
#include <android/log.h>
#include <android/native_activity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_asset(AAssetManager *manager, const char *path) {
    AAsset *asset = AAssetManager_open(manager, path, AASSET_MODE_BUFFER);
    if (!asset) return NULL;
    off_t size = AAsset_getLength(asset);
    char *text = size >= 0 && size <= 65536 ? calloc((size_t)size + 1, 1) : NULL;
    if (text && AAsset_read(asset, text, (size_t)size) != size) {
        free(text);
        text = NULL;
    }
    AAsset_close(asset);
    return text;
}

/* The order stays in sequence.tsv; neither a filename nor array order selects
 * the poem. This first usable slice intentionally has no next-poem control. */
static char *first_poem(AAssetManager *manager) {
    char *sequence = read_asset(manager, "sequence.tsv");
    char *catalog = read_asset(manager, "poems.tsv");
    char *result = NULL, *body = NULL;
    if (!sequence || !catalog) goto done;
    char *row = strchr(sequence, '\n');
    char *id = row ? strchr(row + 1, '\t') : NULL;
    if (!id) goto done;
    ++id;
    id[strcspn(id, "\r\n")] = 0;
    char *saved = NULL;
    for (char *line = strtok_r(catalog, "\n", &saved); line;
         line = strtok_r(NULL, "\n", &saved)) {
        char *fields[8], *cursor = line;
        size_t count = 0;
        while (count < 8) {
            fields[count++] = cursor;
            char *tab = strchr(cursor, '\t');
            if (!tab) break;
            *tab = 0;
            cursor = tab + 1;
        }
        if (count != 8 || strcmp(fields[0], id)) continue;
        if (strcmp(fields[5], "bundled") || strncmp(fields[6], "poems/", 6)
            || strstr(fields[6], "..")) break;
        body = read_asset(manager, fields[6]);
        if (!body) break;
        size_t capacity = strlen(body) + strlen(fields[1]) + strlen(fields[2])
            + strlen(fields[3]) + strlen(fields[4]) + strlen(fields[7]) + 128;
        result = malloc(capacity);
        if (result) snprintf(result, capacity, "%s\n%s\n\n%s\n\n%s\n%s\n\n%s",
                             fields[2], fields[1], body, fields[3], fields[7], fields[4]);
        break;
    }
done:
    free(body);
    free(sequence);
    free(catalog);
    return result;
}

static int jni_failed(JNIEnv *env) {
    if (!(*env)->ExceptionCheck(env)) return 0;
    (*env)->ExceptionDescribe(env);
    (*env)->ExceptionClear(env);
    __android_log_print(ANDROID_LOG_ERROR, "Zine", "Android reader construction failed");
    return 1;
}

void ANativeActivity_onCreate(ANativeActivity *activity, void *saved, size_t saved_size) {
    (void)saved;
    (void)saved_size;
    JNIEnv *env = activity->env; /* NativeActivity invokes this on its UI thread. */
    if ((*env)->PushLocalFrame(env, 32) < 0) return;
    char *poem = first_poem(activity->assetManager);
    jclass text_class = (*env)->FindClass(env, "android/widget/TextView");
    jclass scroll_class = (*env)->FindClass(env, "android/widget/ScrollView");
    jclass activity_class = (*env)->GetObjectClass(env, activity->clazz);
    if (jni_failed(env) || !text_class || !scroll_class) goto done;
    /* NativeActivity defaults to an application-owned drawing surface and
     * input queue. This app uses framework widgets, so return both to Android.
     * Otherwise accessibility sees text while the native surface stays black. */
    jmethodID get_window = (*env)->GetMethodID(env, activity_class, "getWindow", "()Landroid/view/Window;");
    if (jni_failed(env)) goto done;
    jobject window = (*env)->CallObjectMethod(env, activity->clazz, get_window);
    if (jni_failed(env) || !window) goto done;
    jclass window_class = (*env)->GetObjectClass(env, window);
    jmethodID take_surface = (*env)->GetMethodID(env, window_class, "takeSurface", "(Landroid/view/SurfaceHolder$Callback2;)V");
    jmethodID take_input = (*env)->GetMethodID(env, window_class, "takeInputQueue", "(Landroid/view/InputQueue$Callback;)V");
    if (jni_failed(env)) goto done;
    (*env)->CallVoidMethod(env, window, take_surface, NULL);
    (*env)->CallVoidMethod(env, window, take_input, NULL);
    if (jni_failed(env)) goto done;
    jmethodID text_constructor = (*env)->GetMethodID(env, text_class, "<init>", "(Landroid/content/Context;)V");
    jmethodID scroll_constructor = (*env)->GetMethodID(env, scroll_class, "<init>", "(Landroid/content/Context;)V");
    if (jni_failed(env)) goto done;
    jobject text = (*env)->NewObject(env, text_class, text_constructor, activity->clazz);
    jobject scroll = (*env)->NewObject(env, scroll_class, scroll_constructor, activity->clazz);
    if (jni_failed(env) || !text || !scroll) goto done;

    /* Bundled seed texts are BMP UTF-8, also valid JNI modified UTF-8. */
    jstring contents = (*env)->NewStringUTF(env, poem ? poem : "The bundled poem could not be read.");
    jmethodID set_text = (*env)->GetMethodID(env, text_class, "setText", "(Ljava/lang/CharSequence;)V");
    jmethodID set_size = (*env)->GetMethodID(env, text_class, "setTextSize", "(F)V");
    jmethodID set_padding = (*env)->GetMethodID(env, text_class, "setPadding", "(IIII)V");
    jmethodID set_color = (*env)->GetMethodID(env, text_class, "setTextColor", "(I)V");
    jmethodID set_spacing = (*env)->GetMethodID(env, text_class, "setLineSpacing", "(FF)V");
    jmethodID set_selectable = (*env)->GetMethodID(env, text_class, "setTextIsSelectable", "(Z)V");
    jmethodID set_background = (*env)->GetMethodID(env, scroll_class, "setBackgroundColor", "(I)V");
    jmethodID add_view = (*env)->GetMethodID(env, scroll_class, "addView", "(Landroid/view/View;)V");
    jmethodID set_content = (*env)->GetMethodID(env, activity_class, "setContentView", "(Landroid/view/View;)V");
    if (jni_failed(env) || !contents) goto done;
    (*env)->CallVoidMethod(env, text, set_text, contents);
    (*env)->CallVoidMethod(env, text, set_size, 22.0f);
    (*env)->CallVoidMethod(env, text, set_padding, 32, 40, 32, 40);
    (*env)->CallVoidMethod(env, text, set_color, (jint)0xff25231f);
    (*env)->CallVoidMethod(env, text, set_spacing, 0.0f, 1.25f);
    (*env)->CallVoidMethod(env, text, set_selectable, JNI_TRUE);
    (*env)->CallVoidMethod(env, scroll, set_background, (jint)0xfff8f4ea);
    if (jni_failed(env)) goto done;

    jclass typeface_class = (*env)->FindClass(env, "android/graphics/Typeface");
    jfieldID serif_field = (*env)->GetStaticFieldID(env, typeface_class, "SERIF", "Landroid/graphics/Typeface;");
    jmethodID set_typeface = (*env)->GetMethodID(env, text_class, "setTypeface", "(Landroid/graphics/Typeface;)V");
    if (jni_failed(env)) goto done;
    jobject serif = (*env)->GetStaticObjectField(env, typeface_class, serif_field);
    (*env)->CallVoidMethod(env, text, set_typeface, serif);
    jclass linkify = (*env)->FindClass(env, "android/text/util/Linkify");
    jmethodID add_links = (*env)->GetStaticMethodID(env, linkify, "addLinks", "(Landroid/widget/TextView;I)Z");
    if (jni_failed(env)) goto done;
    (*env)->CallStaticBooleanMethod(env, linkify, add_links, text, 1);
    (*env)->CallVoidMethod(env, scroll, add_view, text);
    (*env)->CallVoidMethod(env, activity->clazz, set_content, scroll);
    if (!jni_failed(env)) __android_log_print(ANDROID_LOG_INFO, "Zine", "First poem displayed");
done:
    free(poem);
    (*env)->PopLocalFrame(env, NULL);
}
