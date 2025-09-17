#include "custom_emoji_display.h"
#include "lvgl_theme.h"

#include <esp_log.h>
#include <font_awesome.h>

#include <algorithm>
#include <cstring>
#include <string>

#define TAG "CustomEmojiDisplay"

// Emotion mapping table - map 21 emotions to 6 colorful GIFs
const CustomEmojiDisplay::EmotionMap CustomEmojiDisplay::emotion_maps_[] = {
    // Neutral/calm emotions -> staticstate
    {"neutral", &staticstate},
    {"relaxed", &staticstate},
    {"sleepy", &staticstate},

    // Positive/happy emotions -> happy
    {"happy", &happy},
    {"laughing", &happy},
    {"funny", &happy},
    {"loving", &happy},
    {"confident", &happy},
    {"winking", &happy},
    {"cool", &happy},
    {"delicious", &happy},
    {"kissy", &happy},
    {"silly", &happy},

    // Sad emotions -> sad
    {"sad", &sad},
    {"crying", &sad},

    // Angry emotions -> anger
    {"angry", &anger},

    // Surprised/shocked emotions -> scare
    {"surprised", &scare},
    {"shocked", &scare},
    {"embarrassed", &scare},

    // Thinking/confused emotions -> buxue
    {"thinking", &buxue},
    {"confused", &buxue},

    // End marker
    {nullptr, nullptr}
};

CustomEmojiDisplay::CustomEmojiDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                                     int width, int height, int offset_x, int offset_y, 
                                     bool mirror_x, bool mirror_y, bool swap_xy)
    : SpiLcdDisplay(panel_io, panel, width, height, offset_x, offset_y, mirror_x, mirror_y, swap_xy),
      emotion_gif_(nullptr) {
    
    // Setup GIF container for colorful emotions
    SetupGifContainer();
    
    ESP_LOGI(TAG, "Custom colorful emoji display initialized");
}

void CustomEmojiDisplay::SetupGifContainer() {
    DisplayLockGuard lock(this);

    // Hide default emoji label (we'll use GIF instead)
    if (emoji_label_) {
        lv_obj_add_flag(emoji_label_, LV_OBJ_FLAG_HIDDEN);
    }

    // Create GIF emotion container
    emotion_gif_ = lv_gif_create(content_);
    int gif_size = LV_HOR_RES / 2; // Làm to hơn: chia 2 thay vì chia 3
    lv_obj_set_size(emotion_gif_, gif_size, gif_size);
    lv_obj_set_style_border_width(emotion_gif_, 0, 0);
    lv_obj_set_style_bg_opa(emotion_gif_, LV_OPA_TRANSP, 0);
    lv_obj_center(emotion_gif_);
    
    // Set default emotion
    lv_gif_set_src(emotion_gif_, &staticstate);

    ESP_LOGI(TAG, "GIF emotion container setup completed");
}

void CustomEmojiDisplay::SetEmotion(const char* emotion) {
    if (!emotion || !emotion_gif_) {
        return;
    }

    DisplayLockGuard lock(this);

    // Find matching emotion in mapping table
    for (const auto& map : emotion_maps_) {
        if (map.name && strcmp(map.name, emotion) == 0) {
            lv_gif_set_src(emotion_gif_, map.gif);
            ESP_LOGI(TAG, "Set colorful emotion: %s", emotion);
            return;
        }
    }

    // Default to staticstate if emotion not found
    lv_gif_set_src(emotion_gif_, &staticstate);
    ESP_LOGI(TAG, "Unknown emotion '%s', using default", emotion);
}

void CustomEmojiDisplay::SetChatMessage(const char* role, const char* content) {
    // Use parent implementation for chat messages
    SpiLcdDisplay::SetChatMessage(role, content);
}