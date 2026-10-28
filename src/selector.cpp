#include "main.h"
#include "robotConfigs.h"
#include "selector.h"
#include "autons.h"
#include <atomic>
#include <cstdio>
#include <vector>

LV_IMAGE_DECLARE(steamhappy);

namespace {
const lv_color_t BACKGROUND = lv_color_hex(0x111111);
const lv_color_t PANEL = lv_color_hex(0x1c1c1c);
const lv_color_t BUTTON = lv_color_hex(0x2a2a2a);
const lv_color_t TEXT = lv_color_hex(0xf2f2f2);
const lv_color_t SUBTEXT = lv_color_hex(0x9a9a9a);
const lv_color_t TEAM_RED = lv_color_hex(0xd7261e);
const lv_color_t ALLIANCE_RED = lv_color_hex(0xd7261e);
const lv_color_t ALLIANCE_BLUE = lv_color_hex(0x1f6fe0);

std::atomic<int> selected {0};
std::atomic<bool> red {true};

std::vector<lv_obj_t*> autonButtons;
lv_obj_t* allianceButton;
lv_obj_t* allianceLabel;
lv_obj_t* previewImage;
lv_obj_t* nameLabel;
lv_obj_t* descriptionLabel;
lv_obj_t* poseLabel;

lv_color_t allianceColor() { return red ? ALLIANCE_RED : ALLIANCE_BLUE; }

lv_obj_t* createBox(lv_obj_t* parent, int x, int y, int w, int h, lv_color_t color) {
    lv_obj_t* box = lv_obj_create(parent);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_size(box, w, h);
    lv_obj_set_style_bg_color(box, color, 0);
    lv_obj_set_style_border_width(box, 0, 0);
    lv_obj_set_style_radius(box, 8, 0);
    lv_obj_set_style_pad_all(box, 0, 0);
    lv_obj_remove_flag(box, LV_OBJ_FLAG_SCROLLABLE);
    return box;
}

lv_obj_t* createLabel(lv_obj_t* parent, const char* text, const lv_font_t* font, lv_color_t color) {
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    return label;
}

void styleButton(lv_obj_t* button, lv_color_t color) {
    lv_obj_set_style_bg_color(button, color, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_radius(button, 6, 0);
}

void refresh() {
    for (int i = 0; i < (int)autonButtons.size(); i++) {
        styleButton(autonButtons[i], i == selected ? allianceColor() : BUTTON);
    }
    styleButton(allianceButton, allianceColor());
    lv_label_set_text(allianceLabel, red ? "RED" : "BLUE");

    const Auton& auton = autons[selected];
    lv_image_set_src(previewImage, auton.image ? auton.image : &steamhappy);
    lv_label_set_text(nameLabel, auton.name);
    lv_label_set_text(descriptionLabel, auton.description);
}

void onAutonPressed(lv_event_t* e) {
    selected = (int)(intptr_t)lv_event_get_user_data(e);
    refresh();
}

void onAlliancePressed(lv_event_t*) {
    red = !red;
    refresh();
}

void updatePose(lv_timer_t*) {
    const lemlib::Pose pose = chassis.getPose();
    char text[48];
    snprintf(text, sizeof(text), "x %.1f   y %.1f   h %.1f", pose.x, pose.y, pose.theta);
    lv_label_set_text(poseLabel, text);
}
} // namespace

bool isRedAlliance() { return red; }

void runSelectedAuton() {
    const Auton& auton = autons[selected];
    if (auton.run) auton.run();
}

void selectorInit() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen, BACKGROUND, 0);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* team = createLabel(screen, "55885X", &lv_font_montserrat_24, TEAM_RED);
    lv_obj_set_pos(team, 10, 8);
    lv_obj_t* teamName = createLabel(screen, "INQUISITORS", &lv_font_montserrat_12, SUBTEXT);
    lv_obj_align_to(teamName, team, LV_ALIGN_OUT_RIGHT_BOTTOM, 8, -3);

    allianceButton = lv_button_create(screen);
    lv_obj_set_size(allianceButton, 90, 30);
    lv_obj_align(allianceButton, LV_ALIGN_TOP_RIGHT, -10, 6);
    lv_obj_add_event_cb(allianceButton, onAlliancePressed, LV_EVENT_CLICKED, nullptr);
    allianceLabel = createLabel(allianceButton, "", &lv_font_montserrat_16, TEXT);
    lv_obj_center(allianceLabel);

    lv_obj_t* list = createBox(screen, 10, 44, 180, 188, PANEL);
    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(list, 6, 0);
    lv_obj_set_style_pad_row(list, 6, 0);

    autonButtons.clear();
    for (int i = 0; i < autonCount; i++) {
        lv_obj_t* button = lv_button_create(list);
        lv_obj_set_size(button, lv_pct(100), 38);
        lv_obj_add_event_cb(button, onAutonPressed, LV_EVENT_CLICKED, (void*)(intptr_t)i);
        lv_obj_t* label = createLabel(button, autons[i].name, &lv_font_montserrat_16, TEXT);
        lv_obj_center(label);
        autonButtons.push_back(button);
    }

    lv_obj_t* preview = createBox(screen, 198, 44, 272, 188, PANEL);
    lv_obj_set_style_pad_all(preview, 10, 0);

    previewImage = lv_image_create(preview);
    lv_obj_set_size(previewImage, 110, 110);
    lv_obj_set_pos(previewImage, 0, 0);
    lv_image_set_inner_align(previewImage, LV_IMAGE_ALIGN_CENTER);

    nameLabel = createLabel(preview, "", &lv_font_montserrat_20, TEXT);
    lv_obj_set_pos(nameLabel, 122, 4);
    lv_obj_set_width(nameLabel, 130);
    lv_label_set_long_mode(nameLabel, LV_LABEL_LONG_DOT);

    descriptionLabel = createLabel(preview, "", &lv_font_montserrat_14, SUBTEXT);
    lv_obj_set_pos(descriptionLabel, 122, 34);
    lv_obj_set_width(descriptionLabel, 130);
    lv_label_set_long_mode(descriptionLabel, LV_LABEL_LONG_WRAP);

    poseLabel = createLabel(preview, "", &lv_font_montserrat_14, SUBTEXT);
    lv_obj_align(poseLabel, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    refresh();
    updatePose(nullptr);
    lv_timer_create(updatePose, 100, nullptr);
    lv_screen_load(screen);
}
