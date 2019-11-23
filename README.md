# MonoGUI
Lightweight Graphical User Interface library for monochromatic GLCDs

![Example](https://raw.githubusercontent.com/19greg96/MonoGUI/master/picture.png)

Two screens showing capabilities. Left: Graph component with two signals, and many GraphLabel components. Right: MenuButton, Range, RadioButton, ToggleButton and Sprite components.

Uses ![MonoGFX](https://github.com/19greg96/MonoGFX) to draw components to a byte buffer.

## Features
 - Components can be added to multiple screens
 - Event based interaction
 - Optimized for small displays
 - Many components: Label, Button, ScrollButton, Range, ToggleButton, RadioButton, Menu and Graph
 - Supports fonts generated by ![The Dot Factory](https://github.com/pavius/the-dot-factory)

## Usage

 - Clone this repo, add `Inc/` to include directories and `Src/` to source directories.
 - Install ![MonoGFX](https://github.com/19greg96/MonoGFX) and create a `MonoGFX_DisplayTypedef` object.
 - Call `void MonoGUI_init();`
 - Create a default font:
 
       uint32_t defaultFont = MonoGUI_register_font(&MonoGUI_font_legible3x5_6ptFontInfo);
 - Create your components and add them to screens.
 - In your render loop: handle user input and call any of the following functions based on your input:
   - `void MonoGUI_scroll(int16_t delta, uint8_t largeStep);`
   - `void MonoGUI_tab(int16_t delta);`
   - `void MonoGUI_mainBtnClick();`
   - `void MonoGUI_set_screen(int32_t screen);`
   - `void MonoGUI_select_component(MonoGUI_Component* component);`
   - `void MonoGUI_click_component(MonoGUI_Component* component);`
   
   This is where your callbacks will be **synchronously** called, so it is not advised to call these methods from interrupts.
 - If you are using multiple `MonoGFX_DisplayTypedef` objects, call `void MonoGFX_set_active_display(MonoGFX_DisplayTypedef* disp)` on the one you want to render to.
 - Call `void MonoGUI_render();`. This draws all the components in the current screen to the active MonoGFX buffer.
 - Currently, creating multiple instances of MonoGUI is not supported. To draw to multiple displays from a single controller, create separate screens and select the appropriate screen with `void MonoGUI_set_screen(int32_t screen);` before you call `void MonoGUI_render();`.

## TODO
 - Remove use of `arm_math.h` (and thus `stm32f4xx_hal.h`) or move it behind preprocessor switch
 - More components (TextBox, ScrollBar, ProgressBar, Window, MenuBar, PieChart, BarGraph)
 - More properties to configure existing components
 - More format methods and better implementation
 - Documentation
 - Examples
 - Tests
 - Better Graph component implementation
 - Support component width and height
