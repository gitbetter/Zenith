/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZGLInput.cpp
 
    Created by Adrian Sanchez on 28/01/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.
 
 This file is part of Zenith.
 
 Zenith is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Zenith is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ZGLInput.hpp"
#include "ZEventAgent.hpp"
#include "ZObjectLookEvent.hpp"
#include "ZObjectDragEvent.hpp"
#include "ZObjectMoveEvent.hpp"
#include "ZQuitEvent.hpp"
#include "ZFireEvent.hpp"

void ZGLInput::Initialize() {
    ZInput::Initialize();
    
    keyMap_[ZKEY_SPACE] = GLFW_KEY_SPACE;
    keyMap_[ZKEY_APOSTROPHE] = GLFW_KEY_APOSTROPHE;
    keyMap_[ZKEY_COMMA] = GLFW_KEY_COMMA;
    keyMap_[ZKEY_MINUS] = GLFW_KEY_MINUS;
    keyMap_[ZKEY_PERIOD] = GLFW_KEY_PERIOD;
    keyMap_[ZKEY_SLASH] = GLFW_KEY_SLASH;
    keyMap_[ZKEY_0] = GLFW_KEY_0;
    keyMap_[ZKEY_1] = GLFW_KEY_1;
    keyMap_[ZKEY_2] = GLFW_KEY_2;
    keyMap_[ZKEY_3] = GLFW_KEY_3;
    keyMap_[ZKEY_4] = GLFW_KEY_4;
    keyMap_[ZKEY_5] = GLFW_KEY_5;
    keyMap_[ZKEY_6] = GLFW_KEY_6;
    keyMap_[ZKEY_7] = GLFW_KEY_7;
    keyMap_[ZKEY_8] = GLFW_KEY_8;
    keyMap_[ZKEY_9] = GLFW_KEY_9;
    keyMap_[ZKEY_SEMICOLON] = GLFW_KEY_SEMICOLON;
    keyMap_[ZKEY_EQUAL] = GLFW_KEY_EQUAL;
    keyMap_[ZKEY_A] = GLFW_KEY_A;
    keyMap_[ZKEY_B] = GLFW_KEY_B;
    keyMap_[ZKEY_C] = GLFW_KEY_C;
    keyMap_[ZKEY_D] = GLFW_KEY_D;
    keyMap_[ZKEY_E] = GLFW_KEY_E;
    keyMap_[ZKEY_F] = GLFW_KEY_F;
    keyMap_[ZKEY_G] = GLFW_KEY_G;
    keyMap_[ZKEY_H] = GLFW_KEY_H;
    keyMap_[ZKEY_I] = GLFW_KEY_I;
    keyMap_[ZKEY_J] = GLFW_KEY_J;
    keyMap_[ZKEY_K] = GLFW_KEY_K;
    keyMap_[ZKEY_L] = GLFW_KEY_L;
    keyMap_[ZKEY_M] = GLFW_KEY_M;
    keyMap_[ZKEY_N] = GLFW_KEY_N;
    keyMap_[ZKEY_O] = GLFW_KEY_O;
    keyMap_[ZKEY_P] = GLFW_KEY_P;
    keyMap_[ZKEY_Q] = GLFW_KEY_Q;
    keyMap_[ZKEY_R] = GLFW_KEY_R;
    keyMap_[ZKEY_S] = GLFW_KEY_S;
    keyMap_[ZKEY_T] = GLFW_KEY_T;
    keyMap_[ZKEY_U] = GLFW_KEY_U;
    keyMap_[ZKEY_V] = GLFW_KEY_V;
    keyMap_[ZKEY_W] = GLFW_KEY_W;
    keyMap_[ZKEY_X] = GLFW_KEY_X;
    keyMap_[ZKEY_Y] = GLFW_KEY_Y;
    keyMap_[ZKEY_Z] = GLFW_KEY_Z;
    keyMap_[ZKEY_LEFT_BRACKET] = GLFW_KEY_LEFT_BRACKET;
    keyMap_[ZKEY_BACKSLASH] = GLFW_KEY_BACKSLASH;
    keyMap_[ZKEY_RIGHT_BRACKET] = GLFW_KEY_RIGHT_BRACKET;
    keyMap_[ZKEY_GRAVE_ACCENT] = GLFW_KEY_GRAVE_ACCENT;
    keyMap_[ZKEY_ESCAPE] = GLFW_KEY_ESCAPE;
    keyMap_[ZKEY_ENTER] = GLFW_KEY_ENTER;
    keyMap_[ZKEY_TAB] = GLFW_KEY_TAB;
    keyMap_[ZKEY_BACKSPACE] = GLFW_KEY_BACKSPACE;
    keyMap_[ZKEY_INSERT] = GLFW_KEY_INSERT;
    keyMap_[ZKEY_DELETE] = GLFW_KEY_DELETE;
    keyMap_[ZKEY_RIGHT] = GLFW_KEY_RIGHT;
    keyMap_[ZKEY_LEFT] = GLFW_KEY_LEFT;
    keyMap_[ZKEY_DOWN] = GLFW_KEY_DOWN;
    keyMap_[ZKEY_UP] = GLFW_KEY_UP;
    keyMap_[ZKEY_PAGE_UP] = GLFW_KEY_PAGE_UP;
    keyMap_[ZKEY_PAGE_DOWN] = GLFW_KEY_PAGE_DOWN;
    keyMap_[ZKEY_HOME] = GLFW_KEY_HOME;
    keyMap_[ZKEY_END] = GLFW_KEY_END;
    keyMap_[ZKEY_CAPS_LOCK] = GLFW_KEY_CAPS_LOCK;
    keyMap_[ZKEY_SCROLL_LOCK] = GLFW_KEY_SCROLL_LOCK;
    keyMap_[ZKEY_NUM_LOCK] = GLFW_KEY_NUM_LOCK;
    keyMap_[ZKEY_PRINT_SCREEN] = GLFW_KEY_PRINT_SCREEN;
    keyMap_[ZKEY_PAUSE] = GLFW_KEY_PAUSE;
    keyMap_[ZKEY_F1] = GLFW_KEY_F1;
    keyMap_[ZKEY_F2] = GLFW_KEY_F2;
    keyMap_[ZKEY_F3] = GLFW_KEY_F3;
    keyMap_[ZKEY_F4] = GLFW_KEY_F4;
    keyMap_[ZKEY_F5] = GLFW_KEY_F5;
    keyMap_[ZKEY_F6] = GLFW_KEY_F6;
    keyMap_[ZKEY_F7] = GLFW_KEY_F7;
    keyMap_[ZKEY_F8] = GLFW_KEY_F8;
    keyMap_[ZKEY_F9] = GLFW_KEY_F9;
    keyMap_[ZKEY_F10] = GLFW_KEY_F10;
    keyMap_[ZKEY_F11] = GLFW_KEY_F11;
    keyMap_[ZKEY_F12] = GLFW_KEY_F12;
    keyMap_[ZKEY_F13] = GLFW_KEY_F13;
    keyMap_[ZKEY_F14] = GLFW_KEY_F14;
    keyMap_[ZKEY_F15] = GLFW_KEY_F15;
    keyMap_[ZKEY_F16] = GLFW_KEY_F16;
    keyMap_[ZKEY_F17] = GLFW_KEY_F17;
    keyMap_[ZKEY_F18] = GLFW_KEY_F18;
    keyMap_[ZKEY_F19] = GLFW_KEY_F19;
    keyMap_[ZKEY_F20] = GLFW_KEY_F20;
    keyMap_[ZKEY_F21] = GLFW_KEY_F21;
    keyMap_[ZKEY_F22] = GLFW_KEY_F22;
    keyMap_[ZKEY_F23] = GLFW_KEY_F23;
    keyMap_[ZKEY_F24] = GLFW_KEY_F24;
    keyMap_[ZKEY_F25] = GLFW_KEY_F25;
    keyMap_[ZKEY_KP_0] = GLFW_KEY_KP_0;
    keyMap_[ZKEY_KP_1] = GLFW_KEY_KP_1;
    keyMap_[ZKEY_KP_2] = GLFW_KEY_KP_2;
    keyMap_[ZKEY_KP_3] = GLFW_KEY_KP_3;
    keyMap_[ZKEY_KP_4] = GLFW_KEY_KP_4;
    keyMap_[ZKEY_KP_5] = GLFW_KEY_KP_5;
    keyMap_[ZKEY_KP_6] = GLFW_KEY_KP_6;
    keyMap_[ZKEY_KP_7] = GLFW_KEY_KP_7;
    keyMap_[ZKEY_KP_8] = GLFW_KEY_KP_8;
    keyMap_[ZKEY_KP_9] = GLFW_KEY_KP_9;
    keyMap_[ZKEY_KP_DECIMAL] = GLFW_KEY_KP_DECIMAL;
    keyMap_[ZKEY_KP_DIVIDE] = GLFW_KEY_KP_DIVIDE;
    keyMap_[ZKEY_KP_MULTIPLY] = GLFW_KEY_KP_MULTIPLY;
    keyMap_[ZKEY_KP_SUBTRACT] = GLFW_KEY_KP_SUBTRACT;
    keyMap_[ZKEY_KP_ADD] = GLFW_KEY_KP_ADD;
    keyMap_[ZKEY_KP_ENTER] = GLFW_KEY_KP_ENTER;
    keyMap_[ZKEY_KP_EQUAL] = GLFW_KEY_KP_EQUAL;
    keyMap_[ZKEY_LEFT_SHIFT] = GLFW_KEY_LEFT_SHIFT;
    keyMap_[ZKEY_LEFT_CONTROL] = GLFW_KEY_LEFT_CONTROL;
    keyMap_[ZKEY_LEFT_ALT] = GLFW_KEY_LEFT_ALT;
    keyMap_[ZKEY_LEFT_SUPER] = GLFW_KEY_LEFT_SUPER;
    keyMap_[ZKEY_RIGHT_SHIFT] = GLFW_KEY_RIGHT_SHIFT;
    keyMap_[ZKEY_RIGHT_CONTROL] = GLFW_KEY_RIGHT_CONTROL;
    keyMap_[ZKEY_RIGHT_ALT] = GLFW_KEY_RIGHT_ALT;
    keyMap_[ZKEY_RIGHT_SUPER] = GLFW_KEY_RIGHT_SUPER;
    keyMap_[ZKEY_MENU] = GLFW_KEY_MENU;
    
    mouseMap_[ZMOUSE_LEFT] = GLFW_MOUSE_BUTTON_LEFT;
    mouseMap_[ZMOUSE_RIGHT] = GLFW_MOUSE_BUTTON_RIGHT;
    mouseMap_[ZMOUSE_MIDDLE] = GLFW_MOUSE_BUTTON_MIDDLE;
    mouseMap_[ZMOUSE_4] = GLFW_MOUSE_BUTTON_4;
    mouseMap_[ZMOUSE_5] = GLFW_MOUSE_BUTTON_5;
    mouseMap_[ZMOUSE_6] = GLFW_MOUSE_BUTTON_6;
    mouseMap_[ZMOUSE_7] = GLFW_MOUSE_BUTTON_7;
    mouseMap_[ZMOUSE_8] = GLFW_MOUSE_BUTTON_8;
    mouseMap_[ZMOUSE_LAST] = GLFW_MOUSE_BUTTON_LAST;
}

void ZGLInput::Update() {
    GLFWwindow* windowHandle = glfwGetCurrentContext();

    keyPress_[keyMap_[ZKEY_W]] = glfwGetKey(windowHandle, GLFW_KEY_W) == GLFW_PRESS;
    keyPress_[keyMap_[ZKEY_A]] = glfwGetKey(windowHandle, GLFW_KEY_A) == GLFW_PRESS;
    keyPress_[keyMap_[ZKEY_S]] = glfwGetKey(windowHandle, GLFW_KEY_S) == GLFW_PRESS;
    keyPress_[keyMap_[ZKEY_D]] = glfwGetKey(windowHandle, ZKEY_D) == GLFW_PRESS;
    keyPress_[keyMap_[GLFW_KEY_SPACE]] = glfwGetKey(windowHandle, GLFW_KEY_SPACE) == GLFW_PRESS;
    keyPress_[keyMap_[GLFW_KEY_TAB]] = glfwGetKey(windowHandle, GLFW_KEY_TAB) == GLFW_PRESS;
    keyPress_[keyMap_[GLFW_KEY_ESCAPE]] = glfwGetKey(windowHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    
    mousePress_[mouseMap_[ZMOUSE_LEFT]] = glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    mousePress_[mouseMap_[ZMOUSE_RIGHT]] = glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE;
    mousePress_[mouseMap_[ZMOUSE_MIDDLE]] = glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE;
    
    if (keyPress_[keyMap_[ZKEY_W]]) {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(0.f, 0.f, 1.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[keyMap_[ZKEY_A]]) {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(-1.f, 0.f, 0.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[keyMap_[ZKEY_S]]) {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(0.f, 0.f, -1.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[keyMap_[ZKEY_D]]) {
        std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(1.f, 0.f, 0.f));
        zenith::EventAgent()->TriggerEvent(moveEvent);
    }
    if (keyPress_[keyMap_[GLFW_KEY_ESCAPE]]) {
        std::shared_ptr<ZQuitEvent> quitEvent(new ZQuitEvent);
        zenith::EventAgent()->TriggerEvent(quitEvent);
    }
    
    double yaw, pitch;
    glfwGetCursorPos(windowHandle, &yaw, &pitch);
    
    if (firstLook_) {
        lastYaw_ = yaw; lastPitch_ = pitch;
        firstLook_ = false;
    }
    
    double deltaYaw = yaw - lastYaw_;
    double deltaPitch = lastPitch_ - pitch;
    
    if (mousePress_[mouseMap_[ZMOUSE_LEFT]]) {
        if (deltaYaw != 0.0 || deltaPitch != 0.0) {
            std::shared_ptr<ZObjectDragEvent> dragEvent(new ZObjectDragEvent(deltaYaw, deltaPitch, 0.f));
            zenith::EventAgent()->TriggerEvent(dragEvent);
        } else {
            std::shared_ptr<ZFireEvent> fireEvent(new ZFireEvent(yaw, pitch, 0.f));
            zenith::EventAgent()->TriggerEvent(fireEvent);
        }
    }
    
    if (deltaYaw != 0.0 || deltaPitch != 0.0) {
        std::shared_ptr<ZObjectLookEvent> lookEvent(new ZObjectLookEvent(deltaYaw, deltaPitch));
        zenith::EventAgent()->TriggerEvent(lookEvent);
    }
    
    lastYaw_ = yaw; lastPitch_ = pitch;
}

void ZGLInput::GetCursorPosition(double& x, double& y) {
    GLFWwindow* windowHandle = glfwGetCurrentContext();
    glfwGetCursorPos(windowHandle, &x, &y);
}

void ZGLInput::SetCursorPosition(double& x, double& y) {
    GLFWwindow* windowHandle = glfwGetCurrentContext();
    glfwSetCursorPos(windowHandle, x, y);
}
