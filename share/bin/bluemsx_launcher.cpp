// --------------------------------------------------------------------
// BlueberryMSX launcher
//
// MIT License
// 
// Copyright (c) 2021 HRA!
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// --------------------------------------------------------------------

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <nlohmann/json.hpp>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

using json = nlohmann::json;
using namespace std;

// --------------------------------------------------------------------
static json get_json_file( const string &s_file_name ) {
	json j;

	ifstream file( s_file_name );
	if( !file.is_open() ) {
		return ""_json;
	}
	file >> j;
	return j;
}

// --------------------------------------------------------------------
static vector<string> str_split( const string &s, char c ) {
	stringstream s_target( s );
	string s_segment;
	vector<std::string> s_list;

	while( getline( s_target, s_segment, c ) ) {
		s_list.push_back( s_segment );
	}
	return s_list;
}

// --------------------------------------------------------------------
struct CMENUITEM {
	SDL_Texture *p_texture_unselected;
	SDL_Texture *p_texture_selected;
	SDL_Rect rect;
};

// --------------------------------------------------------------------
class CINFO {
public:
	int i_screen_width;
	int i_screen_height;
	string s_font_name;
	int i_menu_item_font_size;
	int i_info_font_size;
	TTF_Font *p_menu_item_font;
	TTF_Font *p_info_font;
	SDL_Joystick *p_joystick;
	SDL_Window *p_window;
	SDL_Renderer *p_renderer;
	string s_image_info_name;
	string s_image_menu_name;
	SDL_Surface *p_image_info;
	SDL_Surface *p_image_menu;
	CMENUITEM *p_menu_item;
	int i_menu_fadein_red;
	int i_menu_fadein_green;
	int i_menu_fadein_blue;
	int i_menu_fadeout_red;
	int i_menu_fadeout_green;
	int i_menu_fadeout_blue;
	int i_menu_item_red;
	int i_menu_item_green;
	int i_menu_item_blue;
	int i_menu_current_item_red;
	int i_menu_current_item_green;
	int i_menu_current_item_blue;
	int i_current_item;
	int i_top_item;
	int i_menu_x;
	int i_menu_y;
	int i_menu_w;
	int i_menu_h;
	int i_menu_item_height;
	int i_max_items;
	int i_info_x;
	int i_info_y;
	int i_info_w;
	int i_info_h;
	int i_info_item_height;
	int i_info_rom_cartridge_x;
	int i_info_rom_cartridge_y;
	int i_info_rom_cartridge_w;
	int i_info_rom_cartridge_h;
	int i_info_screen_shot_x;
	int i_info_screen_shot_y;
	int i_info_screen_shot_w;
	int i_info_screen_shot_h;
	string s_command;
	string s_shutdown;
	string s_bluemsx_path;
	json j_items;
	int i_total_items;
	vector<string> s_items_keys;

	CINFO():
		i_screen_width(0), 
		i_screen_height(0), 
		s_font_name(""),
		i_menu_item_font_size(0),
		i_info_font_size(0),
		p_menu_item_font(NULL),
		p_info_font(NULL),
		p_joystick(NULL),
		p_window(NULL),
		p_renderer(NULL),
		s_image_info_name(""),
		s_image_menu_name(""),
		p_image_info(NULL),
		p_image_menu(NULL),
		p_menu_item(NULL),
		s_command(""),
		s_shutdown(""),
		s_bluemsx_path("") {
	}

	// ----------------------------------------------------------------
	void set( json &json_data ) {
		this->i_screen_width			= json_data[ "screen width" ].get<int>();
		this->i_screen_height			= json_data[ "screen height" ].get<int>();
		this->s_font_name				= "../font/" + json_data[ "font name" ].get<string>();
		this->i_menu_item_font_size		= json_data[ "menu item font size" ].get<int>();
		this->i_info_font_size			= json_data[ "information font size" ].get<int>();
		this->s_image_info_name			= "../images/" + json_data[ "information image" ].get<string>();
		this->s_image_menu_name			= "../images/" + json_data[ "menu image" ].get<string>();

		auto color = json_data[ "menu fadein color" ];
		this->i_menu_fadein_red			= color[0].get<int>();
		this->i_menu_fadein_green		= color[1].get<int>();
		this->i_menu_fadein_blue		= color[2].get<int>();

		color = json_data[ "menu fadeout color" ];
		this->i_menu_fadeout_red		= color[0].get<int>();
		this->i_menu_fadeout_green		= color[1].get<int>();
		this->i_menu_fadeout_blue		= color[2].get<int>();

		color = json_data[ "menu item color" ];
		this->i_menu_item_red			= color[0].get<int>();
		this->i_menu_item_green			= color[1].get<int>();
		this->i_menu_item_blue			= color[2].get<int>();

		color = json_data[ "menu current item color" ];
		this->i_menu_current_item_red	= color[0].get<int>();
		this->i_menu_current_item_green	= color[1].get<int>();
		this->i_menu_current_item_blue	= color[2].get<int>();
		this->i_current_item			= -1;
		this->i_top_item				= -1;

		auto position = json_data[ "menu item position" ];
		this->i_menu_x					= position[0].get<int>();
		this->i_menu_y					= position[1].get<int>();
		this->i_menu_w					= position[2].get<int>();
		this->i_menu_h					= position[3].get<int>();
		this->i_menu_item_height		= json_data[ "menu item height" ].get<int>();
		this->i_max_items				= this->i_menu_h / this->i_menu_item_height;
		this->p_menu_item				= new CMENUITEM[ this->i_max_items ];
		for( int i = 0; i < this->i_max_items; i++ ) {
			this->p_menu_item[i].p_texture_unselected = nullptr;
			this->p_menu_item[i].p_texture_selected = nullptr;
		}

		this->j_items = get_json_file( "../" + json_data[ "games" ].get<string>() );
		this->i_total_items				= this->j_items.size();
		this->s_items_keys.clear();
		for( auto it = this->j_items.begin(); it != this->j_items.end(); ++it ) {
			this->s_items_keys.push_back( it.key() );
		}

		position = json_data[ "information position" ];
		this->i_info_x					= position[0].get<int>();
		this->i_info_y					= position[1].get<int>();
		this->i_info_w					= position[2].get<int>();
		this->i_info_h					= position[3].get<int>();
		this->i_info_item_height		= json_data[ "information item height" ].get<int>();

		position = json_data[ "imformation rom cartridge" ];
		this->i_info_rom_cartridge_x	= position[0].get<int>();
		this->i_info_rom_cartridge_y	= position[1].get<int>();
		this->i_info_rom_cartridge_w	= position[2].get<int>();
		this->i_info_rom_cartridge_h	= position[3].get<int>();

		position = json_data[ "imformation screen shot" ];
		this->i_info_screen_shot_x		= position[0].get<int>();
		this->i_info_screen_shot_y		= position[1].get<int>();
		this->i_info_screen_shot_w		= position[2].get<int>();
		this->i_info_screen_shot_h		= position[3].get<int>();

		this->s_command					= "";
		this->s_shutdown				= json_data[ "shutdown" ].get<string>();
		this->s_bluemsx_path			= json_data[ "bluemsx path" ].get<string>();
	}

	// ----------------------------------------------------------------
	void get_resources( void ) {
		putchar( '.' );
		SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK );
		putchar( '.' );
		SDL_ShowCursor( SDL_DISABLE );
		putchar( '.' );
		SDL_JoystickEventState( SDL_ENABLE );
		putchar( '.' );

		this->p_menu_item_font			= TTF_OpenFont( this->s_font_name.c_str(), this->i_menu_item_font_size );
		putchar( '.' );
		this->p_info_font				= TTF_OpenFont( this->s_font_name.c_str(), this->i_info_font_size );
		putchar( '.' );
		this->p_joystick				= SDL_JoystickOpen(0);
		putchar( '.' );
		this->p_window					= SDL_CreateWindow( "BlueberryMSX Launcher", 0, 0, this->i_screen_width, this->i_screen_height, 0 );
		putchar( '.' );
		this->p_renderer				= SDL_CreateRenderer( this->p_window, -1, SDL_RENDERER_ACCELERATED );
		putchar( '.' );
		this->p_image_info				= IMG_Load( this->s_image_info_name.c_str() );
		putchar( '.' );
		this->p_image_menu				= IMG_Load( this->s_image_menu_name.c_str() );
		printf( ".\n" );
	}

	// ----------------------------------------------------------------
	void release( void ) {
		if( this->p_menu_item != nullptr ) {
			for( int i = 0; i < this->i_max_items; i++ ) {
				if( this->p_menu_item[i].p_texture_selected != nullptr ) {
					SDL_DestroyTexture( this->p_menu_item[i].p_texture_selected );
					this->p_menu_item[i].p_texture_selected = nullptr;
				}
				if( this->p_menu_item[i].p_texture_unselected != nullptr ) {
					SDL_DestroyTexture( this->p_menu_item[i].p_texture_unselected );
					this->p_menu_item[i].p_texture_unselected = nullptr;
				}
			}
			delete[] this->p_menu_item;
			this->p_menu_item = nullptr;
		}
		if( this->p_joystick != nullptr ) {
			SDL_JoystickClose( this->p_joystick );
			this->p_joystick = nullptr;
		}
		if( this->p_menu_item_font != nullptr ) {
			TTF_CloseFont( this->p_menu_item_font );
			this->p_menu_item_font = nullptr;
		}
		if( this->p_info_font != nullptr ) {
			TTF_CloseFont( this->p_info_font );
			this->p_info_font = nullptr;
		}
		if( this->p_image_menu != nullptr ) {
			SDL_FreeSurface( this->p_image_menu );
			this->p_image_menu = nullptr;
		}
		if( this->p_image_info != nullptr ) {
			SDL_FreeSurface( this->p_image_info );
			this->p_image_info = nullptr;
		}
		if( this->p_renderer != nullptr ) {
			SDL_DestroyRenderer( this->p_renderer );
			this->p_renderer = nullptr;
		}
		if( this->p_window != nullptr ) {
			SDL_DestroyWindow( this->p_window );
			this->p_window = nullptr;
		}
		SDL_Quit();
	}
};

// --------------------------------------------------------------------
static void initialize_program( CINFO *p_info ) {
	IMG_Init( IMG_INIT_JPG | IMG_INIT_PNG );
	TTF_Init();

	json j = get_json_file( "../config.json" );
	p_info->set( j );
	p_info->get_resources();
}

// --------------------------------------------------------------------
static void terminate_program( CINFO *p_info ) {
	p_info->release();
	TTF_Quit();
	IMG_Quit();
}

// -----------------------------------------------------------------------------
static void fadein( CINFO *p_info, SDL_Texture *p_image_texture, int red, int green, int blue ) {
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = p_info->i_screen_width;
	rect.h = p_info->i_screen_height;

	for( int i = 0; i < 256; i += 8 ) {
		SDL_RenderClear( p_info->p_renderer );
		SDL_RenderCopy( p_info->p_renderer, p_image_texture, NULL, &rect );

		SDL_SetRenderDrawBlendMode( p_info->p_renderer, SDL_BLENDMODE_BLEND );
		SDL_SetRenderDrawColor( p_info->p_renderer, red, green, blue, 255 - i );
		SDL_RenderFillRect( p_info->p_renderer, &rect );

		SDL_RenderPresent( p_info->p_renderer );
	}

	SDL_SetRenderDrawBlendMode( p_info->p_renderer, SDL_BLENDMODE_NONE );
	SDL_RenderCopy( p_info->p_renderer, p_image_texture, NULL, &rect );
	SDL_RenderPresent( p_info->p_renderer );
}

// -----------------------------------------------------------------------------
static void fadeout( CINFO *p_info, SDL_Texture *p_image_texture, int red, int green, int blue ) {
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = p_info->i_screen_width;
	rect.h = p_info->i_screen_height;

	for( int i = 0; i < 256; i += 8 ) {
		SDL_RenderClear( p_info->p_renderer );
		SDL_RenderCopy( p_info->p_renderer, p_image_texture, NULL, &rect );

		SDL_SetRenderDrawBlendMode( p_info->p_renderer, SDL_BLENDMODE_BLEND );
		SDL_SetRenderDrawColor( p_info->p_renderer, red, green, blue, i );
		SDL_RenderFillRect( p_info->p_renderer, &rect );

		SDL_RenderPresent( p_info->p_renderer );
	}

	SDL_SetRenderDrawBlendMode( p_info->p_renderer, SDL_BLENDMODE_NONE );
	SDL_RenderFillRect( p_info->p_renderer, &rect );
	SDL_RenderPresent( p_info->p_renderer );
}

// -----------------------------------------------------------------------------
static void renew_textures( CINFO *p_info, int i_last_top_item, int i_line_count ) {
	int l, d, i_line_item;
	string s_title;
	SDL_Surface *p_font_surface;
	SDL_Color item_color	= { (Uint8)p_info->i_menu_item_red, (Uint8)p_info->i_menu_item_green, (Uint8)p_info->i_menu_item_blue };
	SDL_Color current_color	= { (Uint8)p_info->i_menu_current_item_red, (Uint8)p_info->i_menu_current_item_green, (Uint8)p_info->i_menu_current_item_blue };

	// 重複している部分があれば再利用する 
	if( i_last_top_item == p_info->i_top_item ) {
		return;
	}
	if( (i_last_top_item < p_info->i_top_item) && ((i_last_top_item + i_line_count) >= p_info->i_top_item ) ) {
		// top が下へ移動した場合 
		d = p_info->i_top_item - i_last_top_item;
		for( l = 0; l < i_line_count; l++ ) {
			if( l < d ) {
				if( p_info->p_menu_item[l].p_texture_unselected != nullptr ) {
					SDL_DestroyTexture( p_info->p_menu_item[l].p_texture_unselected );
					p_info->p_menu_item[l].p_texture_unselected = nullptr;
				}
				if( p_info->p_menu_item[l].p_texture_selected != nullptr ) {
					SDL_DestroyTexture( p_info->p_menu_item[l].p_texture_selected );
					p_info->p_menu_item[l].p_texture_selected = nullptr;
				}
			}
			if( l < (i_line_count - d) ) {
				p_info->p_menu_item[l] = p_info->p_menu_item[l + d];
			}
			else {
				p_info->p_menu_item[l].p_texture_unselected = nullptr;
				p_info->p_menu_item[l].p_texture_selected = nullptr;
			}
		}
	}
	else if( (i_last_top_item > p_info->i_top_item) && ((p_info->i_top_item + i_line_count) >= i_last_top_item ) ) {
		// top が上へ移動した場合 
		d = i_last_top_item - p_info->i_top_item;
		for( l = i_line_count - 1; l >= 0; l-- ) {
			if( l >= (i_line_count - d) ) {
				if( p_info->p_menu_item[l].p_texture_unselected != nullptr ) {
					SDL_DestroyTexture( p_info->p_menu_item[l].p_texture_unselected );
					p_info->p_menu_item[l].p_texture_unselected = nullptr;
				}
				if( p_info->p_menu_item[l].p_texture_selected != nullptr ) {
					SDL_DestroyTexture( p_info->p_menu_item[l].p_texture_selected );
					p_info->p_menu_item[l].p_texture_selected = nullptr;
				}
			}
			if( l < d ) {
				p_info->p_menu_item[l].p_texture_unselected = nullptr;
				p_info->p_menu_item[l].p_texture_selected = nullptr;
			}
			else {
				p_info->p_menu_item[l] = p_info->p_menu_item[l - d];
			}
		}
	}
	else {
		// ページチェンジ(全更新)の場合
		for( l = 0; l < i_line_count; l++ ) {
			if( p_info->p_menu_item[l].p_texture_unselected != nullptr ) {
				SDL_DestroyTexture( p_info->p_menu_item[l].p_texture_unselected );
				p_info->p_menu_item[l].p_texture_unselected = nullptr;
			}
			if( p_info->p_menu_item[l].p_texture_selected != nullptr ) {
				SDL_DestroyTexture( p_info->p_menu_item[l].p_texture_selected );
				p_info->p_menu_item[l].p_texture_selected = nullptr;
			}
		}
	}

	for( l = 0; l < i_line_count; l++ ) {
		if( p_info->p_menu_item[l].p_texture_unselected != nullptr ) {
			continue;
		}
		i_line_item = p_info->i_top_item + l;
		s_title = p_info->j_items[ p_info->s_items_keys[ i_line_item ] ][ "title" ].get<string>();
		p_font_surface = TTF_RenderUTF8_Solid( p_info->p_menu_item_font, s_title.c_str(), item_color );
		p_info->p_menu_item[l].p_texture_unselected = SDL_CreateTextureFromSurface( p_info->p_renderer, p_font_surface );
		SDL_GetClipRect( p_font_surface, &(p_info->p_menu_item[l].rect) );
		SDL_FreeSurface( p_font_surface );

		p_font_surface = TTF_RenderUTF8_Solid( p_info->p_menu_item_font, s_title.c_str(), current_color );
		p_info->p_menu_item[l].p_texture_selected = SDL_CreateTextureFromSurface( p_info->p_renderer, p_font_surface );
		SDL_FreeSurface( p_font_surface );
	}
}

// -----------------------------------------------------------------------------
static void put_menu_item( CINFO *p_info, int i_next_item, int animation ) {
	int l;
	int i_line_item;
	SDL_Color color;
	SDL_Rect dst_rect, dst_rect_anime;
	static const int anime_data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, -1, -2, -3, -4, -5, -6, -7, -6, -5, -4, -3, -2, -1, 0, 0 };

	int i_last_current_item = p_info->i_current_item;
	int i_last_top_item = p_info->i_top_item;
	if( i_next_item < p_info->i_top_item || p_info->i_top_item < 0 ) {
		p_info->i_top_item = i_next_item;
	}
	else if( i_next_item >= (p_info->i_top_item + p_info->i_max_items) ) {
		p_info->i_top_item = i_next_item - p_info->i_max_items + 1;
	}
	if( (p_info->i_top_item + p_info->i_max_items) >= p_info->i_total_items ) {
		p_info->i_top_item = p_info->i_total_items - p_info->i_max_items;
	}
	if( p_info->i_top_item < 0 ) {
		p_info->i_top_item = 0;
	}

	int i_line_count;
	if( (int)p_info->s_items_keys.size() < p_info->i_max_items ) {
		i_line_count = (int)p_info->s_items_keys.size();
	}
	else {
		i_line_count = p_info->i_max_items;
	}
	renew_textures( p_info, i_last_top_item, i_line_count );

	dst_rect.x = p_info->i_menu_x;
	dst_rect.y = p_info->i_menu_y;
	dst_rect.w = p_info->i_menu_w;
	dst_rect.h = p_info->i_menu_item_font_size;

	for( int l = 0; l < i_line_count; l++ ) {
		i_line_item = p_info->i_top_item + l;
		if( p_info->p_menu_item[l].rect.w > p_info->i_menu_w ) {
			dst_rect.w = p_info->i_menu_w;
		}
		else {
			dst_rect.w = p_info->p_menu_item[l].rect.w;
		}
		if( i_line_item == i_next_item ) {
			dst_rect_anime.x = dst_rect.x - anime_data[ animation ];
			dst_rect_anime.y = dst_rect.y - anime_data[ animation ];
			dst_rect_anime.w = dst_rect.w + anime_data[ animation ] * 2;
			dst_rect_anime.h = dst_rect.h + anime_data[ animation ] * 2;
			SDL_RenderCopy( p_info->p_renderer, p_info->p_menu_item[l].p_texture_selected, NULL, &dst_rect_anime );
		}
		else {
			SDL_RenderCopy( p_info->p_renderer, p_info->p_menu_item[l].p_texture_unselected, NULL, &dst_rect );
		}
		dst_rect.y += p_info->i_menu_item_height;
	}
}

// -----------------------------------------------------------------------------
static void put_info( CINFO *p_info ) {
	SDL_Rect src_rect;
	SDL_Rect dst_rect;
	SDL_Surface *p_surface;
	SDL_Texture *p_texture;
	string s_file_name;

	SDL_Color item_color = { (Uint8)p_info->i_menu_item_red, (Uint8)p_info->i_menu_item_green, (Uint8)p_info->i_menu_item_blue };

	json target_info = p_info->j_items[ p_info->s_items_keys[p_info->i_current_item] ];

	string s_information = target_info["information"].get<string>();
	vector<string> a_information = str_split( s_information, '|' );

	// ROM Cartridge image
	s_file_name = target_info["cartridge image"].get<string>();
	if( s_file_name == "" ) {
		s_file_name = "cartridge.png";			// "No Image"
	}
	s_file_name = "../images/" + s_file_name;
	p_surface = IMG_Load( s_file_name.c_str() );
	p_texture = SDL_CreateTextureFromSurface( p_info->p_renderer, p_surface );

	dst_rect.x = p_info->i_info_rom_cartridge_x;
	dst_rect.y = p_info->i_info_rom_cartridge_y;
	dst_rect.w = p_info->i_info_rom_cartridge_w;
	dst_rect.h = p_info->i_info_rom_cartridge_h;
	SDL_RenderCopy( p_info->p_renderer, p_texture, NULL, &dst_rect );

	SDL_FreeSurface( p_surface );
	SDL_DestroyTexture( p_texture );

	// Screen shot image
	s_file_name = target_info["screen shot"].get<string>();
	if( s_file_name == "" ) {
		s_file_name = "screen_shot.png";		// "No Image"
	}
	s_file_name = "../images/" + s_file_name;
	p_surface = IMG_Load( s_file_name.c_str() );
	p_texture = SDL_CreateTextureFromSurface( p_info->p_renderer, p_surface );

	dst_rect.x = p_info->i_info_screen_shot_x;
	dst_rect.y = p_info->i_info_screen_shot_y;
	dst_rect.w = p_info->i_info_screen_shot_w;
	dst_rect.h = p_info->i_info_screen_shot_h;
	SDL_RenderCopy( p_info->p_renderer, p_texture, NULL, &dst_rect );

	SDL_FreeSurface( p_surface );
	SDL_DestroyTexture( p_texture );

	// Information text
	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.w = p_info->i_screen_width;
	dst_rect.h = p_info->i_screen_height;

	dst_rect.x = p_info->i_info_x;
	dst_rect.y = p_info->i_info_y;
	for( auto s_info_line: a_information ) {
		SDL_Surface *p_font_surface = TTF_RenderUTF8_Solid( p_info->p_info_font, s_info_line.c_str(), item_color );
		SDL_GetClipRect( p_font_surface, &src_rect );
		dst_rect.w = src_rect.w;
		dst_rect.h = src_rect.h;
		if( dst_rect.w > p_info->i_info_w ) {
			dst_rect.w = p_info->i_info_w;
		}
		p_texture = SDL_CreateTextureFromSurface( p_info->p_renderer, p_font_surface );
		SDL_RenderCopy( p_info->p_renderer, p_texture, NULL, &dst_rect );
		SDL_FreeSurface( p_font_surface );
		SDL_DestroyTexture( p_texture );

		dst_rect.y = dst_rect.y + p_info->i_info_item_height;
	}
}

// -----------------------------------------------------------------------------
static void menu_selected( CINFO *p_info ) {
	string s_game_file = p_info->s_items_keys[ p_info->i_current_item ];
	json j_current_info = p_info->j_items[ s_game_file ];
	p_info->s_command = j_current_info[ "options" ].get<string>() + " /share/roms/" + s_game_file;
}

// -----------------------------------------------------------------------------
static int menu_up( CINFO *p_info ) {
	int next_current_item;

	if( p_info->i_current_item > 0 ) {
		next_current_item = p_info->i_current_item - 1;
	}
	else {
		next_current_item = p_info->i_total_items - 1;
	}
	return next_current_item;
}

// -----------------------------------------------------------------------------
static int menu_down( CINFO *p_info ) {
	int next_current_item;

	if( p_info->i_current_item < (p_info->i_total_items - 1) ) {
		next_current_item = p_info->i_current_item + 1;
	}
	else {
		next_current_item = 0;
	}
	return next_current_item;
}

// -----------------------------------------------------------------------------
static int menu_pageup( CINFO *p_info ) {
	int next_current_item = 0;

	if( p_info->i_current_item > 0 ) {
		next_current_item = p_info->i_current_item - p_info->i_max_items;
		if( next_current_item < 0 ) {
			next_current_item = 0;
		}
	}
	else {
		next_current_item = p_info->i_total_items - 1;
	}
	return next_current_item;
}

// -----------------------------------------------------------------------------
static int menu_pagedown( CINFO *p_info ) {
	int next_current_item;

	if( p_info->i_current_item < (p_info->i_total_items - 1) ) {
		next_current_item = p_info->i_current_item + p_info->i_max_items;
		if( next_current_item > (p_info->i_total_items - 1) ) {
			next_current_item = p_info->i_total_items - 1;
		}
	}
	else {
		next_current_item = 0;
	}
	return next_current_item;
}

// -----------------------------------------------------------------------------
static bool event_proc_for_menu( CINFO *p_info ) {
	SDL_Event event;
	int animation = 0;

	SDL_Texture *p_image_menu = SDL_CreateTextureFromSurface( p_info->p_renderer, p_info->p_image_menu );
	fadein( p_info, p_image_menu, p_info->i_menu_fadeout_red, p_info->i_menu_fadeout_green, p_info->i_menu_fadeout_blue );

	// 余計な event を読み捨てる 
	while( SDL_PollEvent( &event ) );

	int next_current_item = 0;
	bool is_running = true;
	bool is_menu_running = true;
	while( is_menu_running ) {
		while( SDL_PollEvent( &event ) ) {
			if( event.type == SDL_QUIT ) {
				is_running = false;
				break;
			}
			else if( event.type == SDL_KEYDOWN ) {
				if( event.key.keysym.sym == SDLK_ESCAPE ) {
					is_running = false;
					is_menu_running = false;
					break;
				}
				else if( event.key.keysym.sym == SDLK_SPACE ) {
					menu_selected( p_info );
					is_menu_running = false;
					break;
				}
				else if( event.key.keysym.sym == SDLK_UP ) {
					next_current_item = menu_up( p_info );
				}
				else if( event.key.keysym.sym == SDLK_LEFT ) {
					next_current_item = menu_pageup( p_info );
				}
				else if( event.key.keysym.sym == SDLK_DOWN ) {
					next_current_item = menu_down( p_info );
				}
				else if( event.key.keysym.sym == SDLK_RIGHT ) {
					next_current_item = menu_pagedown( p_info );
				}
			}
			else if( event.type == SDL_JOYBUTTONDOWN ) {
				if( event.jbutton.button == 0 ) {
					menu_selected( p_info );
					is_menu_running = false;
					break;
				}
				else if( event.jbutton.button == 1 ) {
					is_running = false;
					is_menu_running = false;
				}
				break;
			}
			else if( event.type == SDL_JOYAXISMOTION ) {
				if( event.jaxis.axis == 1 && event.jaxis.value <= -16384 ) {
					next_current_item = menu_up( p_info );
				}
				else if( event.jaxis.axis == 0 && event.jaxis.value <= -16384 ) {
					next_current_item = menu_pageup( p_info );
				}
				else if( event.jaxis.axis == 1 && event.jaxis.value >= 16384 ) {
					next_current_item = menu_down( p_info );
				}
				else if( event.jaxis.axis == 0 && event.jaxis.value >= 16384 ) {
					next_current_item = menu_pagedown( p_info );
				}
			}
		}
		SDL_RenderCopy( p_info->p_renderer, p_image_menu, NULL, NULL );
		put_menu_item( p_info, next_current_item, animation );
		animation = (animation + 1) & 31;
		p_info->i_current_item = next_current_item;
		SDL_RenderPresent( p_info->p_renderer );
	}

	fadeout( p_info, p_image_menu, p_info->i_menu_fadeout_red, p_info->i_menu_fadeout_green, p_info->i_menu_fadeout_blue );
	SDL_DestroyTexture( p_image_menu );
	return is_running;
}

// -----------------------------------------------------------------------------
static bool event_proc_for_info( CINFO *p_info ) {
	SDL_Event event;
	SDL_Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = p_info->i_screen_width;
	rect.h = p_info->i_screen_height;

	SDL_Texture *p_image_info = SDL_CreateTextureFromSurface( p_info->p_renderer, p_info->p_image_info );
	fadein( p_info, p_image_info, p_info->i_menu_fadein_red, p_info->i_menu_fadein_green, p_info->i_menu_fadein_blue );

	SDL_RenderClear( p_info->p_renderer );
	SDL_RenderCopy( p_info->p_renderer, p_image_info, NULL, &rect );
	put_info( p_info );
	SDL_RenderPresent( p_info->p_renderer );

	// 余計な event を読み捨てる 
	while( SDL_PollEvent( &event ) );

	// ボタン入力待ち 
	bool is_accept = false;
	for(;;) {
		if( SDL_WaitEvent( &event ) ) {
			if( event.type == SDL_QUIT ) {
				break;
			}
			else if( event.type == SDL_KEYDOWN ) {
				if( event.key.keysym.sym == SDLK_SPACE ) {
					is_accept = true;
					break;
				}
				if( event.key.keysym.sym == SDLK_ESCAPE ) {
					break;
				}
			}
			else if( event.type == SDL_JOYBUTTONDOWN ) {
				if( event.jbutton.button == 0 ) {
					is_accept = true;
					break;
				}
				if( event.jbutton.button == 1 ) {
					break;
				}
			}
		}
	}

	fadeout( p_info, p_image_info, p_info->i_menu_fadeout_red, p_info->i_menu_fadeout_green, p_info->i_menu_fadeout_blue );
	SDL_DestroyTexture( p_image_info );
	return is_accept;
}

// --------------------------------------------------------------------
int main( int argc, char *argv[] ) {
	printf( "BlueberryMSX launcher for RaspberryPiZero\n" );
	printf( "==========================================================\n" );
	printf( "Programmed by t.hara, 30th/Dec./2021\n" );

	CINFO info;

	initialize_program( &info );

	for(;;) {
		if( !event_proc_for_menu( &info ) ) {
			break;
		}

		if( !event_proc_for_info( &info ) ) {
			continue;
		}
		//	SDLを解放する
		terminate_program( &info );
		//	blueberryMSXを起動する
		printf( "%s\n", info.s_bluemsx_path.c_str() );
		filesystem::current_path( info.s_bluemsx_path );
		string s_command = "./bluemsx-pi " + info.s_command;
		printf( "s_command [%s]", s_command.c_str() );
		system( s_command.c_str() );
		filesystem::current_path( "/share/bin" );
		exit(0);
	}
	terminate_program( &info );

	if( info.s_shutdown == "" ) {
		return 1;
	}
	else {
		system( info.s_shutdown.c_str() );
	}
	return 0;
}
