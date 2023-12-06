---@meta


---
---Allows operation on Windows
---@class window
local window = {}

local all_windows = {}

---
---Change the window title.
---
---@param title string
function window.set_title(title) end

---@alias window.windowmode
---| "normal"
---| "minimized"
---| "maximized"
---| "fullscreen"

---
---Change the window mode.
---
---@param mode window.windowmode
function window.set_mode(mode) end

---
---Retrieve the current window mode.
---
---@return window.windowmode mode
function window.get_mode() end

---
---Toggle between bordered and borderless.
---
---@param bordered boolean
function window.set_bordered(bordered) end

---
---When then window is run borderless (without system decorations), this
---function allows to set the size of the different regions that allow
---for custom window management.
---To disable custom window management, call this function without any
---arguments
---
---@param title_height? number Height of the window decoration
---@param controls_width? number Width of window controls (maximize,minimize and close buttons, etc).
---@param resize_border? number The amount of pixels reserved for resizing
function window.set_window_hit_test(title_height, controls_width, resize_border) end

---
---Get the size and coordinates of the window.
---
---@return number width
---@return number height
---@return number x
---@return number y
function window.get_size() end

---
---Sets the size and coordinates of the window.
---
---@param width number
---@param height number
---@param x number
---@param y number
function window.set_size(width, height, x, y) end

---
--- Returns if the window has input focus.
---
--- @return boolean
function window.has_focus() end

---
---Raise a window above other windows and set the input focus.
---Note: in some environments this may be ignored
function window.focus() end

---
---Change the opacity (also known as transparency) of the window.
---
---@param opacity number A value from 0.0 to 1.0, the lower the value
---the less visible the window will be.
---@return boolean success True if the operation suceeded.
function window.set_window_opacity(opacity) end
