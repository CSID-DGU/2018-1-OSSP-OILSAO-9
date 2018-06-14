#include "game_states.h"
#include <time.h>


SDL_Rect item;
SDL_Surface* item_life;
SDL_Surface* item_shield;
SDL_Surface* item_player_speed;
bool item_exist = false;
int showRanking();
void save_score(int score, int quick_check);
void make_id();

void menu()
{
	int mode = INITIAL_MODE;
	int level = 0;

	while (mode != EXIT)
	{
		switch (mode)
		{
		case INITIAL_MODE:
			mode = select_mode();
			break;
		case SINGLE_MODE:
			mode = select_level();
			break;
		case MULTI_MODE:
			mode = socketing();
			break;
		case RANKING_MODE:
			mode = showRanking();
			break;
			case SEARCH_MODE:
			mode=searchRanking();
			break;
		default:
			break;
		}
	}
}

int select_mode()
{
	bool quit = false;
	int mode = 0;
	while (quit == false)
	{
		if (SDL_PollEvent(&event))
		{
			apply_surface(0, 0, background, screen);
			title_message = TTF_RenderText_Solid(font2, "Awesome Dodge", textColor);
			apply_surface((640 - title_message->w) / 2, 80, title_message, screen);
			message = TTF_RenderText_Solid(font, "Press space to start, esc key to quit", textColor);
			apply_surface((640 - message->w) / 2, 480 / 2 - message->h, message, screen);
			message = TTF_RenderText_Solid(font, "Single     Multi        Ranking", textColor);
			apply_surface((640 - message->w) / 2, 480 / 2 + message->h, message, screen);
			message2 = TTF_RenderText_Solid(font, "Single     ", textColor);
			int tmp = message2->w;
			message2 = TTF_RenderText_Solid(font, ">", textColor);
			apply_surface((640 - message->w) / 2 - 8 + mode * tmp, 480 / 2 + message->h, message2, screen);
			SDL_Flip(screen);
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_RIGHT:
				{
					if (mode >= 2) break;
					mode++;
					break;
				}
				case SDLK_LEFT:
				{
					if (mode <= 0) break;
					mode--;
					break;
				}
				case SDLK_SPACE:
				{
					message = NULL;
					if (mode == 1) return MULTI_MODE;
					else if (mode == 0) return SINGLE_MODE;
					else if (mode == 2) return RANKING_MODE;
					break;
				}
				case SDLK_ESCAPE://esc 키가 눌리면 종료
					return EXIT;
					break;
				default:
					break;
				}
			}
			else if (event.type == SDL_QUIT)
			{
				return EXIT;
				quit = true;
			}
		}
	}
}

int select_level()
{
	bool quit = false;
	int selector = 0;
	while (quit == false)
	{
		if (SDL_PollEvent(&event))
		{
			message = TTF_RenderText_Solid(font, "Press space to start, esc key to quit", textColor);
			apply_surface(0, 0, background, screen);
			title_message = TTF_RenderText_Solid(font2, "Awesome Dodge", textColor);
			apply_surface((640 - title_message->w) / 2, 100, title_message, screen);
			apply_surface((640 - message->w) / 2, 480 / 2 - message->h, message, screen);
			message = TTF_RenderText_Solid(font, "level 1         level 2        level 3", textColor);
			apply_surface((640 - message->w) / 2, 480 / 2 + message->h, message, screen);
			message2 = TTF_RenderText_Solid(font, "level 1         ", textColor);
			int tmp = message2->w;
			message2 = TTF_RenderText_Solid(font, ">", textColor);
			apply_surface((640 - message->w) / 2 - 8 + selector * tmp, 480 / 2 + message->h, message2, screen);
			SDL_Flip(screen);
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_RIGHT:
				{
					if (selector >= 2) break;
					selector++;
					break;
				}
				case SDLK_LEFT:
				{
					if (selector <= 0) break;
					selector--;
					break;
				}
				case SDLK_SPACE:
				{
					message = NULL;
					init();
					main_game(selector, SINGLE_MODE);
					return INITIAL_MODE;
					break;
				}
				case SDLK_ESCAPE://esc 키가 눌리면 종료
					return INITIAL_MODE;
					break;
				default:
					break;
				}
			}
			else if (event.type == SDL_QUIT)
			{
				return EXIT;
				quit = true;
			}
		}
	}
}

int socketing()
{
	client = socket(AF_INET, SOCK_STREAM, 0);
	int yes = 1;
	if(setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) std::cout << "socket setting error" << std::endl;

	if (client < 0)
	{
		std::cout << "\n소켓 준비 에러..." << std::endl;
	}

	std::cout << "\n=> 소켓 생성 완료..." << std::endl;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portNum);

	bool isServer = true;
	int count = 0;
	inet_pton(AF_INET, ip, &server_addr.sin_addr);

	bool isConnect = false;
	bool* cntPointer = &isConnect;
	std::thread waitingMassage(waiting, &cntPointer);
	while (true)
	{
		if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) != -1)
		{
			isServer = false;
			isConnect = true;
			waitingMassage.join();
		}
		count++;
		if (count == 3) break;
	}

	if (isServer)
	{
		inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr); // 초기값인 0.0.0.0으로 초기화

		while ((bind(client, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0)
		{
			//std::cout << "=> Error binding connection, the socket has already been established..." << std::endl;
			//count = (count + 1) % 4;

			//std::string str = "Server is Creating, Esc key to quit";
			//message = TTF_RenderText_Solid(font, str.c_str(), textColor);
			//apply_surface(0, 0, background, screen);
			//apply_surface((640 - message->w) / 2, 480 / 2 - message->h, message, screen);
			//SDL_Delay(500);
			//std::string str2 = "Waiting";
			//for (int j = 0; j < count; j++) str2 += " .";
			//message2 = TTF_RenderText_Solid(font, str2.c_str(), textColor);
			//apply_surface((640 - message2->w) / 2, 480 / 2 - message2->h + message->h + 10, message2, screen);
			SDL_Flip(screen);
			if (SDL_PollEvent(&event))
			{
				std::string str = "Server is Creating, Esc key to quit";
				message = TTF_RenderText_Solid(font, str.c_str(), textColor);
				apply_surface(0, 0, background, screen);
				apply_surface((640 - message->w) / 2, 480 / 2 - message->h, message, screen);

				if (event.type == SDL_KEYDOWN)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_ESCAPE://esc 키가 눌리면 종료
						return INITIAL_MODE;
						break;
					default:
						break;
					}
				}
				else if (event.type == SDL_QUIT)
				{
					return EXIT;
				}
			}
		}
		server_addr.sin_addr.s_addr = htons(INADDR_ANY);

		size = sizeof(server_addr);
		std::cout << "=> Looking for clients..." << std::endl;
		server = -1;

		listen(client, 1);

		std::thread listenFor(waitClient, &cntPointer);

		while (!isConnect)
		{
			if (SDL_PollEvent(&event))
			{
				if (event.type == SDL_KEYDOWN)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_ESCAPE://esc 키가 눌리면 종료
						isConnect = true;
						int tmp_client;
						tmp_client = socket(AF_INET, SOCK_STREAM, 0);
						inet_pton(AF_INET, ip, &server_addr.sin_addr);
						if (connect(tmp_client, (struct sockaddr*)&server_addr, size) == -1)
						{
							//error message
							std::cout << "Connecting Error!\nServer was not created." << std::endl;
							exit(1);
						}

						//3번 반복
						//int count = 0;
						/*while (true)
						{
						if (connect(tmp_client, (struct sockaddr *)&server_addr, sizeof(server_addr)) != -1)
						std::cout << "success" << std::endl;
						else std::cout << "fail" << std::endl;
						count++;
						if (count == 3) break;
						}*/
						close(tmp_client);
						close(client);
						close(server);
						listenFor.join();
						waitingMassage.join();
						return INITIAL_MODE;
						break;
					default:
						break;
					}
				}
				else if (event.type == SDL_QUIT)
				{
					return EXIT;
				}
			}
		}
		listenFor.join();
		waitingMassage.join();

		/*count = -1;
		while (true)
		{
			server = accept(client, (struct sockaddr *)&server_addr, &size);
			if(server >= 0) break;
			count = (count + 1) % 4;
			if (waiting(count) == INITIAL_MODE) return INITIAL_MODE;
		}*/
		/* ------------- ACCEPTING CLIENTS  ------------- */
		/* ----------------- listen() ------------------- */

		// first check if it is valid or not
		/*
		if (server < 0)
			std::cout << "=> Error on accepting..." << std::endl;
		*/

		buffer_int[0] = (unsigned int)time(NULL);
		send(server, buffer_int, bufsize, 0);
		srand(buffer_int[0]);
		message = NULL;
		init();
		main_game(0, SERVER_MODE);
		listenFor.~thread();
	}
	else
	{
		inet_pton(AF_INET, ip, &server_addr.sin_addr);
		/*
		while (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
		{}
		*/
		std::cout << "연결 완료!" << std::endl;
		std::cout << "=> 연결된 서버 포트 번호: " << portNum << std::endl;
		int iResult;
		struct timeval tv;
		bool timeout = false;
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(client, &rfds);

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		iResult = select(client + 1, &rfds, (fd_set *)0, (fd_set *)0, &tv);
		if (iResult > 0)
		{
			recv(client, buffer_int, bufsize, 0);
			timeout = true;
		}
		if (!timeout) return INITIAL_MODE;

		srand(buffer_int[0]);
		message = NULL;
		init();
		main_game(0, CLIENT_MODE);
	}

	return INITIAL_MODE;
}

void waiting(bool **isConnect)
{
	int count = 0;
	while (!(**isConnect))
	{
		std::string str = "Waiting";
		for (int j = 0; j < count; j++) str += " .";
		message = TTF_RenderText_Solid(font, str.c_str(), textColor);
		apply_surface(0, 0, background, screen);
		apply_surface((640 - message->w) / 2, 480 / 2 - message->h, message, screen);
		SDL_Flip(screen);
		SDL_Delay(500);
		count = (count + 1) % 4;
	}
}

void waitClient(bool **isConnect)
{
	 server = accept(client, (struct sockaddr *)&server_addr, &size);
	 **isConnect = true;
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return false;
	}
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

	if (screen == NULL)
	{
		return false;
	}

	if (TTF_Init() == -1)
	{
		return false;
	}
	srand(time(NULL));
	SDL_WM_SetCaption("Awesome", NULL);

	return true;
}

bool load_files()
{

	background = load_image("assets/background.png");
	font = TTF_OpenFont("assets/BMDOHYEON_ttf.ttf", 24);
	font2 = TTF_OpenFont("assets/RaphLanokFuture.otf", 48);

	player = SDL_LoadBMP("assets/player1.bmp");
	player2 = SDL_LoadBMP("assets/player2.bmp");
	ball = load_image("assets/rocket.bmp");
	heart = SDL_LoadBMP("assets/heart.bmp");
	enemy_heart = SDL_LoadBMP("assets/enemy_heart.bmp");

        //item 관련 아이콘추가
        item_life = SDL_LoadBMP("assets/life.bmp");//라이프 증가 아이템
        item_shield = SDL_LoadBMP("assets/enemy_ball.bmp");//아이콘 만들어서 수정하기
        item_player_speed = SDL_LoadBMP("assets/enemy_heart.bmp");//아이콘 만들어서 수정하


	if (background == NULL)
	{
		return false;
	}

	if (font == NULL)
	{
		return false;
	}

	return true;
}

void clean_up()
{
	SDL_FreeSurface(background);
	SDL_FreeSurface(message);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(ball);

	TTF_CloseFont(font);
	TTF_Quit();

	SDL_Quit();
}

//아이템 랜덤으로 생성하는 함수
int item_make()

{
	item;//header파일에 SDL_Rect으로 선언되어 있음.
	item.x = BALL_SIZE / 2 + rand() % (SCREEN_WIDTH - BALL_SIZE / 2);
//SCREEN_WIDTH/2;
	item.y = 10;
	item.w = item.h = BALL_SIZE;

        int random;
        srand(time(NULL));
        random = rand()%100+1;

        //item1
        if(random >0 && random <=10) return 1; // 라이프 증가 아이템 생성
	else if(random >10 && random <=25) return 2; // 쉴드 아이템
        else if(random >25 && random <=35) return 3; // 플레이어 속도 증가
        else if(random >35 && random <=50) return 4; // 플레이어 속도 감소

        //item2
	else if(random >50 && random <=60) return 5; // 공 크기 증가
	else if(random >60 && random <=70) return 6; // 공 크기 감소
	else if(random >70 && random <=90) return 7; // 공 속도 증가
        else if(random >90 && random <=100) return 8; // 공 속도 감소


}






void main_game(int selector, int mode)//난이도 선택 변수
{
	using namespace std;
	bool quit = false;
	//client side player
	int player_position = SCREEN_WIDTH / 2;
	int player_position_y = SCREEN_HEIGHT - 3;
	//server side player
	int player2_position = SCREEN_WIDTH / 2;
	int player2_position_y = SCREEN_HEIGHT - 3;
	Uint8 *keystates = NULL;
	int start_time = SDL_GetTicks();
	int level = 1 + selector; // level 정의
	int life = 1; // life 추가
	int enemy_life = 3;
	int current_balls = 0;
	int i = 0;
	int Die_Count = 0;

	int fps_timer = 0;
	int delay = 0;
	int frames = 0;
	int fps_calc_timer = SDL_GetTicks();
	int score = 0;

        int item_num = 0;
	int item_check;
        int time2 = SDL_GetTicks();
        int random2 = 0;

	int shield_check = 0;
	int shield_start = 0;

	int player_speed_check = 0;
	int player_speed_start = 0;

	int life_check = 0;


	int randomball[MAX_BALLS]; // 떨어지는 볼의 속도를 랜덤하게 조정하기 위해 선언한 배열

	if (mode == SINGLE_MODE) srand((unsigned int)time(NULL)); //in Single Mode set random ball

	for (i = 0; i < MAX_BALLS; i++)
		randomball[i] = 0;

	init_ball();

	while (quit == false)
	{
		for (i = 0; i < current_balls; i++)
		{
			randomball[i] = (double)rand() / RAND_MAX * (level - 1) + BALL_VELOCITY; // 초기 속도와 레벨 사이의 난수 생성
		}

		fps_timer = SDL_GetTicks();
		if (SDL_GetTicks() - start_time > 1)
		{
			start_time = SDL_GetTicks();
			for (i = 0; i < current_balls; i++)
			{
				balls[i].y += randomball[i];//level증가를 위해서 기존 값에 level을 곱해줌
			}
			item.y+= 1;//item.y 위치 1씩 추가하여 이동
		}
		if (current_balls < MAX_BALLS)
		{
			for (i = 0; i < MAX_BALLS; i++)
			{
				if (balls[i].y > SCREEN_HEIGHT || balls[i].y == 0)
				{
					SDL_Rect new_ball;
					new_ball.x = BALL_SIZE / 2 + rand() % (SCREEN_WIDTH - BALL_SIZE / 2);
					new_ball.y = -(5 + rand() % 350);
					new_ball.w = new_ball.h = BALL_SIZE;
					balls[i] = new_ball;

				}
			}
			current_balls = MAX_BALLS;
		}
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				balls[0].x = player_position;
				balls[0].y = player_position_y;
				life = 1;
				Die_Count = 0;
			}
		}

		keystates = SDL_GetKeyState(NULL);

		if (keystates[SDLK_ESCAPE])
		{
			balls[0].x = player_position;
			balls[0].y = player_position_y;
			life = 1;
			Die_Count = 0;
		}

		if (keystates[SDLK_LEFT] && player_position > PLAYER_WIDTH / 2)
		{
			if(player_speed_check == 0)
				player_position--;
			else if(player_speed_check == 1)
				player_position -= 2;
			else // player_position == -1
				player_position -= 0.5;

		}
		if (keystates[SDLK_RIGHT] && player_position < SCREEN_WIDTH - PLAYER_WIDTH / 2 && player_speed_check == 0)
		{
			if(player_speed_check == 0)
				player_position++;
			else if(player_speed_check == 1)
				player_position += 2;
			else // player_position == -1
				player_position += 0.5;
		}

		if (keystates[SDLK_UP] && player_position_y > PLAYER_HEIGHT / 2 && player_speed_check == 0)
		{
			if(player_speed_check == 0)
				player_position_y--;
			else if(player_speed_check == 1)
				player_position_y -= 2;
			else // player_position == -1
				player_position_y -= 0.5;
		}

		if (keystates[SDLK_DOWN] && player_position_y < SCREEN_HEIGHT - PLAYER_HEIGHT / 2 && player_speed_check == 0)
		{
			if(player_speed_check == 0)
				player_position_y++;
			else if(player_speed_check == 1)
				player_position_y += 2;
			else // player_position == -1
				player_position_y += 0.5;
		}//위 아래 이동 추가 //아이템에 따라서 플레이어 속도 조절함.


		apply_surface(0, 0, background, screen);
		if (life == 3) {
			apply_surface(500, 20, heart, screen); apply_surface(540, 20, heart, screen); apply_surface(580, 20, heart, screen);
		}
		// heart decrease as life goes down
		else if (life == 2) {
			apply_surface(540, 20, heart, screen); apply_surface(580, 20, heart, screen);
		}
		else if (life == 1) {
			apply_surface(580, 20, heart, screen);
		}


		if(SDL_GetTicks()-time2 > 1){
		   time2 = SDL_GetTicks();

             	   if(item_exist == false) item_check =0;
             	   else item_check = -1;
		   if((score % 10) == 0  && (item_check == 0)){//임의의 점수마다 아이템을 생성한다.

			item_num = item_make();

                        item_exist = true;

		    }

		    //item_num별로 다른 아이템 아이콘을 출력한다.
		    if(item_num == 1){
			   apply_surface(item.x, item.y, item_life, screen);
			   SDL_SetColorKey(item_life, SDL_SRCCOLORKEY, SDL_MapRGB(item_life->format, 255, 255, 255));
		    }
                    else if(item_num ==2)
			   apply_surface(item.x, item.y, item_shield, screen);
                    else if(item_num ==3 || item_num ==4)
			   apply_surface(item.x, item.y, item_player_speed, screen);


		}


		if (item.y > SCREEN_HEIGHT) item_exist = false;

		for (i = 0; i < MAX_BALLS; i++)
		{
			// printf("ball %i: %i %i\n",i , balls[i].x, balls[i].y);
			apply_surface(balls[i].x, balls[i].y, ball, screen);//판정을 위해서 고친 부분
			if (balls[i].y > SCREEN_HEIGHT)
			{
				current_balls--;
				score++;
				if (score > LEVEL_UP_COUNT * (level - selector))
				{
					level++;//score의 상태에 따라 레벨 증가
				}
			}
			SDL_Rect player_rect;
			SDL_Rect player_rect2;
			player_rect.x = player_position - PLAYER_WIDTH / 2;
			player_rect.y = player_position_y - PLAYER_HEIGHT / 2;
			player_rect.w = PLAYER_WIDTH;
			player_rect.h = PLAYER_HEIGHT;
			player_rect2.x = player2_position - PLAYER_WIDTH / 2;
			player_rect2.y = player2_position_y - PLAYER_HEIGHT / 2;
			player_rect2.w = PLAYER_WIDTH;
			player_rect2.h = PLAYER_HEIGHT;


			if(intersects(item, player_rect)){//플레이어와 아이템이 충돌했는지 여부를 확인, 충동하면 1을 반환하여 조건문 안의 코드 실행.
			// item_num에 따라서 다르게 기능을 실행해야 한다.
				item_exist = false;
			    if(item_num == 1) life_check = 1;
			   //라이프 증가 함수
         	           else if(item_num ==2){//쉴드
				shield_check = 1;

				shield_start = SDL_GetTicks();
			   //쉴드 함수 --> 쉴드는 플레이어가 ball에 닿아도 일정시간동안은 player의 life가 줄어들지 않게 한다.
			  //추가로 플레이어가 쉴드인 상태일때의 아이콘을 바꿔주면 좋을것 같다.
			   }
       		           else if(item_num ==3 || item_num ==4) {
			   //플레이어 속도 조절 함수
				int player_speed_random = 0;
				player_speed_random = rand() % 10 + 1;
				if(player_speed_random >0 && player_speed_random <6) player_speed_check = 1;
				else player_speed_check = -1;
				player_speed_start = SDL_GetTicks();
			   }

			}

			if (intersects(balls[i], player_rect) && Die_Count == 0 && shield_check == 0)
			{
				life--;

/*
				if(life <=0 && mode == SINGLE_MODE) {
					std::stringstream caption;
					std::stringstream caption2;
					std::stringstream caption3;
					std::stringstream temp;
						apply_surface(0, 0, background, screen);
						message = TTF_RenderText_Solid(font, "Game over", textColor);
						apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 - message->h, message, screen);
						caption << "Level : " << level;
						message = TTF_RenderText_Solid(font, caption.str().c_str(), textColor);
						apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h, message, screen);
						caption2 << "Score is : " << score;
						message = TTF_RenderText_Solid(font, caption2.str().c_str(), textColor);
						apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h + 50, message, screen);
						caption3 << "If you want to save score, press space bar";
						message = TTF_RenderText_Solid(font, caption3.str().c_str(), textColor);
						apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h + 100, message, screen);
						SDL_Flip(screen);
							if(SDL_PollEvent(&event)) {
							if(event.type == SDL_KEYDOWN) {
								switch(event.key.keysym.sym) {
									case SDLK_SPACE:
										apply_surface(0, 0, background, screen);//background로 덮음
										temp <<"space bar ok";
										message = TTF_RenderText_Solid(font, temp.str().c_str(), textColor);
										apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h + 100, message, screen);
										//출력 완료 확인.
										//아이디 만드는 함수를 만들어서 출력하기
										save_score(score);
										//화면은 출력하지만, 계속  비행기와 ball이 뜨고
										//출력이 끝나면 바로 창이 메인화면이나 게임화면으로 돌아가는것 고치
									//	SDL_Flip(screen);
								//	case SDLK_DOWN: quit = true;
								}
							}
							}

//1.싱그모드가 종료하면 결과와 press the space bar를 추력
//2.스페이스 바를 누르면 랭킹 저장 화면으로 전환
//3.랭킹 저장 화면에서 아이디를 10글자 이하로 입력받는다
//4.입력받은 아이디와 score을 받아서 db에 저장한다.

				}

*/
				if (life <= 0) //life소진시 종료
				{
					if (enemy_life != 0)
						switch (mode)
						{
							//server side
						case SERVER_MODE:
							std::cout << "SERVER SIDE :";
							std::cout << "Last Client: ";
							recv(server, buffer_int, bufsize, 0);
							player2_position = buffer_int[0];
							player2_position_y = buffer_int[1];
							enemy_life = buffer_int[2];
							std::cout << buffer_int[0] << " " << buffer_int[1] << " " << buffer_int[2] << std::endl;
							buffer_int[0] = player_position;
							buffer_int[1] = player_position_y;
							buffer_int[2] = life;
							std::cout << "Last Server: ";
							std::cout << buffer_int[0] << " " << buffer_int[1] << " " << buffer_int[2] << std::endl;
							send(server, buffer_int, bufsize, 0);
							break;
							//client side
						case CLIENT_MODE:
							std::cout << "CLIENT SIDE :";
							std::cout << "Last Client: ";
							buffer_int[0] = player_position;
							buffer_int[1] = player_position_y;
							buffer_int[2] = life;
							std::cout << buffer_int[0] << " " << buffer_int[1] << " " << buffer_int[2] << std::endl;
							send(client, buffer_int, bufsize, 0);
							std::cout << "Last Server: ";
							recv(client, buffer_int, bufsize, 0);
							player2_position = buffer_int[0];
							player2_position_y = buffer_int[1];
							enemy_life = buffer_int[2];
							std::cout << buffer_int[0] << " " << buffer_int[1] << " " << buffer_int[2] << std::endl;
							break;
						default:
							break;
						}
						close(server);
						close(client);

					if(mode == MULTI_MODE){
						game_over(level, score, LOSER);
						quit = true;

					}
					else if(mode == SINGLE_MODE) {
						game_over(level,score, SINGLE_MODE);
						quit = true;

					}
					else //life가 남아있으면 공 초기화후 계속
					{
						Die_Count++;
					}
				}//if life<=의 조건문 끝.
			}
		}


		if(SDL_GetTicks() - shield_start >= 1000) shield_check = 0;//쉴드시간 완료하면 shield_check를 0으로 표시.
		if(SDL_GetTicks() - player_speed_start >= 1500) player_speed_check = 0;
		if(life_check == 1) {
			if(life >=3) life_check = -1;
			else if(life == 2) {life = 3; life_check = -1; }
			else if(life == 1) {life = 2; life_check = -1; }
		}

		if (Die_Count == 0 || Die_Count % 2 == 0)
		{
			if (Die_Count >= 600) Die_Count = 0;
			if (shield_check == 0) {
			   apply_surface(player_position - PLAYER_WIDTH / 2, player_position_y - PLAYER_HEIGHT / 2/*SCREEN_HEIGHT - PLAYER_HEIGHT*/, player, screen);//player표시를 이동에 따라 표시
			SDL_SetColorKey(player, SDL_SRCCOLORKEY, SDL_MapRGB(player->format, 255, 255, 255));
			}
			else if(shield_check == 1) {//shield인 상태에서 표시되는 player 아이콘을 다르게 한다.
			  apply_surface(player_position - PLAYER_WIDTH / 2, player_position_y - PLAYER_HEIGHT / 2/*SCREEN_HEIGHT - PLAYER_HEIGHT*/, heart, screen);//player표시를 이동에 따라 표시
			  SDL_SetColorKey(player, SDL_SRCCOLORKEY, SDL_MapRGB(player->format, 255, 255, 255));
			}
		}
		if (Die_Count > 0) Die_Count++;

		if ((mode == CLIENT_MODE || mode == SERVER_MODE) && enemy_life > 0)//Socket 일때 만 표시
		{
			apply_surface(player2_position - PLAYER_WIDTH / 2, player2_position_y - PLAYER_HEIGHT / 2/*SCREEN_HEIGHT - PLAYER_HEIGHT*/, player2, screen);//player2표시를 이동에 따라 표시
			SDL_SetColorKey(player2, SDL_SRCCOLORKEY, SDL_MapRGB(player2->format, 255, 255, 255));
			// Present enemy_life on screen
			if (enemy_life == 3) {
			apply_surface(500, 60, enemy_heart, screen); apply_surface(540, 60, enemy_heart, screen); apply_surface(580, 60, enemy_heart, screen);
			}
			else if (enemy_life == 2) {
			apply_surface(540, 60, enemy_heart, screen); apply_surface(580, 60, enemy_heart, screen);
			}
			else if (enemy_life == 1) {
			apply_surface(580, 60, enemy_heart, screen);
			}

		}



		std::stringstream caption, caption2;
		caption << /* "FPS: " << (int)(frames*1000.0/(SDL_GetTicks() - fps_calc_timer+1)) << */"Score: " << score
			<< "       Level: " << level;//level 추가로 표시
		message = TTF_RenderText_Solid(font, caption.str().c_str(), textColor);
		caption2 << "Life: " << life;
		message2 = TTF_RenderText_Solid(font, caption2.str().c_str(), textColor);
		if (SDL_GetTicks() - fps_calc_timer > 5000)
		{
			frames = 1;
			fps_calc_timer = SDL_GetTicks();
		}
		apply_surface(10, 10, message, screen);

		SDL_Flip(screen);
		frames++;

		delay = SDL_GetTicks() - fps_timer;

		/*  Socket 통신을 위한 부분 추가  */
		if (enemy_life != 0 && life != 0)
			switch (mode)
			{
				//server side
			case SERVER_MODE:
				//std::cout << "Client: ";
				recv(server, buffer_int, bufsize, 0);
				player2_position = buffer_int[0];
				player2_position_y = buffer_int[1];
				enemy_life = buffer_int[2];
				//std::cout << buffer_int[0] << " " << buffer_int[1] << " " << buffer_int[2] << std::endl;
				buffer_int[0] = player_position;
				buffer_int[1] = player_position_y;
				buffer_int[2] = life;
				caption2 << "Enemy Life: " << enemy_life;
				apply_surface(SCREEN_WIDTH - 10 - message2->w, 10, message2, screen);
				//std::cout << "Server: ";
				//std::cout << buffer_int[0] << " " << buffer_int[1] << " " << buffer_int[2] << std::endl;
				send(server, buffer_int, bufsize, 0);
				break;

				//client side
			case CLIENT_MODE:
				//std::cout << "Client: ";
				buffer_int[0] = player_position;
				buffer_int[1] = player_position_y;
				buffer_int[2] = life;
				//std::cout << buffer_int[0] << " " << buffer_int[1] << " " << buffer_int[2] << std::endl;
				send(client, buffer_int, bufsize, 0);
				//std::cout << "Server: ";
				recv(client, buffer_int, bufsize, 0);
				player2_position = buffer_int[0];
				player2_position_y = buffer_int[1];
				enemy_life = buffer_int[2];
				caption2 << "Enemy Life: " << enemy_life;
				apply_surface(SCREEN_WIDTH - 10 - message2->w, 10, message2, screen);
				//std::cout << buffer_int[0] << " " << buffer_int[1] << " " << buffer_int[2] << std::endl;
				break;
			case SINGLE_MODE:
				break;
			}

		if(life != 0 && enemy_life == 0 && (mode == SERVER_MODE || mode == CLIENT_MODE))
		{
			shutdown(client, SHUT_RDWR);
			shutdown(server, SHUT_RDWR);
			close(client);
			close(server);
			game_over(level, score, WINNER);//1 == WIN_CASE
			quit = true;
		}

		if (delay < 1000 / FRAMES_PER_SECOND)
		{
			SDL_Delay((1000 / FRAMES_PER_SECOND) - delay);
		}
	}

//싱글모드에서 게임이 종료되고, 랭킹에 점수를 저장하기 위한 화면을 구현.
	if(quit == true && mode == SINGLE_MODE) {
		int quick_check = 0;
		save_score(score, quick_check);
	while(!SDL_PollEvent(&event)) {//입력이 있을때까지 기다린다.
		;//do nothing
	}
	int id_ok_check = 0;

	string id;
	int id_count =0;

//	if(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {
				case SDLK_SPACE: {//make id 창 출력하게 하기.

				while(id_ok_check == 0) {

/*
					for(int i=0;i<=10;i++) {
					std::stringstream caption;
					string temp = to_string(i);
					apply_surface(0, 0, background, screen);
					message = TTF_RenderText_Solid(font, temp.c_str(), textColor);
					apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 4 - message->h, message, screen);
					SDL_Flip(screen);
					}

*/

				std::stringstream caption;
				caption << "make id (id must in 10 alpabet";
				apply_surface(0, 0, background, screen);
				message = TTF_RenderText_Solid(font, caption.str().c_str(), textColor);
				apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 3 - message->h, message, screen);
				if(id_count ==0){
					message = TTF_RenderText_Solid(font, "no id, yet", textColor);
					apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 3 + message->h, message, screen);
				}
				else {
					apply_surface(0, 0, background, screen);
					message = TTF_RenderText_Solid(font, id.c_str(), textColor);
					apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 3 + message->h, message, screen);
				}
				SDL_Flip(screen);
				if(SDL_PollEvent(&event)) {
				if(event.type == SDL_KEYDOWN) {
					switch(event.key.keysym.sym) {
						case SDLK_a:
							id[id_count] = 'a';
							id_count++;
						case SDLK_b:
							id[id_count] = 'b';
							id_count++;

						case SDLK_c:
							id[id_count] = 'c';
							id_count++;

						case SDLK_d:
							id[id_count] = 'd';
							id_count++;

						case SDLK_e:
							id[id_count] = 'e';
							id_count++;

						case SDLK_f:
							id[id_count] = 'f';
							id_count++;

						case SDLK_g:
							id[id_count] = 'g';
							id_count++;

						case SDLK_h:
							id[id_count] = 'h';
							id_count++;
						case SDLK_i:
							id[id_count] = 'i';
							id_count++;
						case SDLK_j:
							id[id_count] = 'j';
							id_count++;

						case SDLK_k:
							id[id_count] = 'k';
							id_count++;

						case SDLK_l:
							id[id_count] = 'l';
							id_count++;

						case SDLK_m:
							id[id_count] = 'm';
							id_count++;

						case SDLK_n:
							id[id_count] = 'n';
							id_count++;

						case SDLK_o:
							id[id_count] = 'o';
							id_count++;

						case SDLK_p:
							id[id_count] = 'p';
							id_count++;
						case SDLK_q:
							id[id_count] = 'q';
							id_count++;
						case SDLK_r:
							id[id_count] = 'r';
							id_count++;

						case SDLK_s:
							id[id_count] = 's';
							id_count++;

						case SDLK_t:
							id[id_count] = 't';
							id_count++;

						case SDLK_u:
							id[id_count] = 'u';
							id_count++;

						case SDLK_v:
							id[id_count] = 'v';
							id_count++;

						case SDLK_w:
							id[id_count] = 'w';
							id_count++;

						case SDLK_x:
							id[id_count] = 'x';
							id_count++;
						case SDLK_y:
							id[id_count] = 'y';
							id_count++;
						case SDLK_z:
							id[id_count] = 'z';
							id_count++;

						////////숫자 입력받음

						case SDLK_0:
							id[id_count] = 't';
							id_count++;

						case SDLK_1:
							id[id_count] = 'u';
							id_count++;

						case SDLK_2:
							id[id_count] = 'v';
							id_count++;

						case SDLK_3:
							id[id_count] = 'w';
							id_count++;

						case SDLK_4:
							id[id_count] = 'x';
							id_count++;
						case SDLK_5:
							id[id_count] = 'y';
							id_count++;
						case SDLK_6:
							id[id_count] = 'z';
							id_count++;
						case SDLK_7:
							id[id_count] = 'x';
							id_count++;
						case SDLK_8:
							id[id_count] = 'y';
							id_count++;
						case SDLK_9:
							id[id_count] = 'z';
							id_count++;

					}
				}//if(event.type == SDL_KEYDOWN)의 괄호 닫기 (AAAAAAAA적혀있는)
				}//while문의 괄호 닫
				}//if(event.type == SDL_KEYDOWN)의 괄호 닫기 (make id적혀있는)

				}//case sdlk_space 괄호 닫음
				case SDLK_DOWN: {}

			}
		}
//	}



	}


}

void init_ball()
{
	for (int i = 0; i < MAX_BALLS; i++)
	{
		SDL_Rect new_ball;
		new_ball.x = BALL_SIZE / 2 + rand() % (SCREEN_WIDTH - BALL_SIZE / 2);
		new_ball.y = -(5 + rand() % 350);
		new_ball.w = new_ball.h = BALL_SIZE;
		balls[i] = new_ball;
	}
}

void game_over(int level, int score, int state)
{
	std::stringstream caption;
	std::stringstream caption2;
	std::stringstream caption3;
	std::stringstream temp;
	switch (state)
	{
		//SINGLE_MODE
		case SINGLE_MODE:
		apply_surface(0, 0, background, screen);
		message = TTF_RenderText_Solid(font, "Game over", textColor);
		apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 - message->h, message, screen);
		caption << "Level : " << level;
		message = TTF_RenderText_Solid(font, caption.str().c_str(), textColor);
		apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h, message, screen);
		caption2 << "Score is : " << score;
		message = TTF_RenderText_Solid(font, caption2.str().c_str(), textColor);
		apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h + 50, message, screen);
		caption3 << "If you want to save score, press space bar";
		message = TTF_RenderText_Solid(font, caption3.str().c_str(), textColor);
		apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h + 100, message, screen);
		SDL_Flip(screen);
		break;
		// 1 == WIN_CASE
		case WINNER:
		apply_surface(0, 0, background, screen);
		message = TTF_RenderText_Solid(font, "! ! ! YOU WIN ! ! !", textColor);
		apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 - message->h, message, screen);
		SDL_Flip(screen);
		break;
		// 2 == LOSE_CASE
		case LOSER:
		apply_surface(0, 0, background, screen);
		message = TTF_RenderText_Solid(font, "( T . T ) YOU LOSE ( T . T )", textColor);
		apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 - message->h, message, screen);
		SDL_Flip(screen);
		break;
	}
/*
	if(state !=SINGLE_MODE) {
		while (true)
		{
			if (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT ||
					(event.type == SDL_KEYDOWN &&
					(event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_ESCAPE)))
				{
					break;
				}
			}
		}
	}
	else if(state == SINGLE_MODE) {;


	}
*/
}


//아이디 출력 창
void save_score(int score, int quit_check) {
	std::stringstream caption;
	std::stringstream caption2;
	std::stringstream caption3;
	std::stringstream caption4;


	apply_surface(0, 0, background, screen);
	message = TTF_RenderText_Solid(font, "Game over", textColor);
	apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 - message->h, message, screen);
	caption2 << "Score is : " << score;
	message = TTF_RenderText_Solid(font, caption2.str().c_str(), textColor);
	apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h + 50, message, screen);
	caption << "save the ID & scroe --> space bar";
	message = TTF_RenderText_Solid(font, caption.str().c_str(), textColor);
	apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h, message, screen);
//	caption3 << "quit --> ESC";
//	message = TTF_RenderText_Solid(font, caption3.str().c_str(), textColor);
//	apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 + message->h, message, screen);

	SDL_Flip(screen);

	int id_make_ok =0;
	int quit_timer=0;


}

//아이디 만드는 창 화면 전환하는 함수.
void make_id() {

	std::stringstream caption;
	std::stringstream caption2;
	std::stringstream caption3;
	std::stringstream caption4;


	apply_surface(0, 0, background, screen);
	message = TTF_RenderText_Solid(font, "make_id_fuction", textColor);
	apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 2 - message->h, message, screen);

	SDL_Flip(screen);

}
//아이디 입력을 받는데, SAVE 버튼이 눌리거나, 아이디를 10글자 받으면 아이디 입력을 마치고 메인 화면으로 올라간다.

//DB와 연동한다.
// int connectDB(){
//   MYSQL *conn =NULL;
// 	MYSQL_RES *res;
// 	MYSQL_ROW row;
//
// 	if(!(conn = mysql_init((MYSQL*)NULL))){
// 		//초기화 함수. 실패시 나간다.
// 		return -1;
// 		exit(1);
// 	}
//
// 	if(!mysql_real_connect(conn,DB_HOST,DB_USER, DB_PASS,DB_NAME, 3306, (char *)NULL, 0)){
// 		printf("connect error.\n");//DB접속 실패.
// 		return -1;
// 		exit(1);
// 	}
//
// 	// if(mysql_select_db(conn, databaseName)!=0){
// 	// 	mysql_close(conn);	//db 선택 실패?
// 	// 	printf("select db fail\n");
// 	// 	return -1;
// 	// 	exit(1);
// 	// }
// 	return 0;
//
// }

int checki=0;
int i=0;

int showRanking(){
	bool quit = false;
	int rankmode = 0;
	MYSQL *conn =NULL;
MYSQL* connection=NULL;
	MYSQL_RES *sql_result;
MYSQL_ROW sql_row;
//conn = mysql_init((MYSQL*)NULL);
std::string DBtoArr[100][3];
if(!(conn = mysql_init((MYSQL*)NULL))){
	//초기화 함수. 실패시 나간다.
	//return -1;
	exit(1);
}
connection = mysql_real_connect(conn,DB_HOST,DB_USER, DB_PASS,DB_NAME, 3306, (char *)NULL, 0);
if(connection==NULL){
	std::cout << "\n데이터 베이스 접속 에러..."<<mysql_error(conn) << std::endl;
	//printf(stderr, "connect error : %s\n", mysql_error(conn));//DB접속 실패.
	//return -1;
	//exit(1);
}
else std:: cout<<"데이터베이스 접속 완료.."<<std::endl;


//OILSAODODGE 데이터 베이스에서 랭킹 정보를 불러온다.
int queryStart;
//connection = mysql_real_connect(conn,DB_HOST,DB_USER, DB_PASS,DB_NAME, 3306, (char *)NULL, 0);
queryStart=mysql_query(connection,"select * from DodgeRank ORDER BY score DESC");
if(queryStart!=0){std::cout<<"쿼리 입력 오류"<<std::endl;fprintf (stderr,"Mysql query error : %s", mysql_error(conn));}
else{std::cout<<"쿼리 입력 완료"<<std::endl;}

sql_result=mysql_store_result(connection);
int j = 0;
while((sql_row=mysql_fetch_row(sql_result)) != NULL){	//DB 정보를 이차원 배열에 저장
	DBtoArr[j][0]=j+1;
	DBtoArr[j][1]=sql_row[1];
	DBtoArr[j][2]=sql_row[2];
	j++;
}
int rankpage=0;
int lrank=0;
mysql_close(conn);

	while(quit == false)
	{
		if(SDL_PollEvent(&event))
		{


			apply_surface(0,0,background,screen);
			message=TTF_RenderText_Solid(font, "RANKING", textColor);
			apply_surface((SCREEN_WIDTH-message->w)/2,SCREEN_HEIGHT/2-message->h-150,message,screen);

			message=TTF_RenderText_Solid(font, "RANK", textColor);
			apply_surface((SCREEN_WIDTH-message->w)/5,SCREEN_HEIGHT/2-message->h-50,message,screen);
			message=TTF_RenderText_Solid(font, "SCORE", textColor);
			apply_surface((SCREEN_WIDTH-message->w)/2,SCREEN_HEIGHT/2-message->h-50,message,screen);
			message=TTF_RenderText_Solid(font, "ID", textColor);
			apply_surface((SCREEN_WIDTH-message->w)/2+200,SCREEN_HEIGHT/2-message->h-50,message,screen);

			std::string caption[10];




			while(lrank < 3) {
				message = TTF_RenderText_Solid(font, std::to_string(DBtoArr[rankpage+lrank][0]).c_str(), textColor);
				apply_surface((SCREEN_WIDTH-message->w)/5,SCREEN_HEIGHT/2-message->h+50*lrank,message,screen);

				message = TTF_RenderText_Solid(font, DBtoArr[rankpage+lrank][1].c_str(), textColor);
				apply_surface((SCREEN_WIDTH-message->w)/2,SCREEN_HEIGHT/2-message->h+50*lrank,message,screen);

				message = TTF_RenderText_Solid(font, DBtoArr[rankpage+lrank][2].c_str(), textColor);
				apply_surface((SCREEN_WIDTH-message->w)/2+200,SCREEN_HEIGHT/2-message->h+50*lrank,message,screen);
				lrank++;
				}
				lrank=0;
			message = TTF_RenderText_Solid(font,  "Search           Exit", textColor);
			apply_surface((SCREEN_WIDTH-message->w)/2,SCREEN_HEIGHT/2-message->h+180,message,screen);
			message2 = TTF_RenderText_Solid(font, "Search           ", textColor);
			int tmp = message2->w;
			message2 = TTF_RenderText_Solid(font, ">", textColor);
			apply_surface((SCREEN_WIDTH-message->w)/2-8+rankmode*tmp,SCREEN_HEIGHT/2-message->h+180,message2,screen);


			SDL_Flip(screen);


			if(event.type == SDL_KEYDOWN)
			{
				switch(event.key.keysym.sym)
				{
				case SDLK_UP:
				{
					if(rankpage<=0)break;
					rankpage--;
					break;
				}
				case SDLK_DOWN:
				{
					if(rankpage>=j)break;
					rankpage++;
					break;
				}
				case SDLK_RIGHT:
				{
					if(rankmode >= 1) break;
					rankmode++;
					break;
				}
				case SDLK_LEFT:
				{
					if(rankmode <= 0) break;
					rankmode--;
					break;
				}
				case SDLK_SPACE:
				{
					message = NULL;
					if(rankmode == 0) return SEARCH_MODE;
					else if(rankmode == 1) return INITIAL_MODE;

					break;
				}
				case SDLK_ESCAPE:
					return EXIT;
					break;
				default:
					break;
				}
			}
			else if(event.type == SDL_QUIT)
			{
				return EXIT;
				quit = true;
			}

		}
	}
}
int searchRanking(){
	std::string id="";
	int id_count =0;
while(true){
//	if(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {

				std::stringstream caption;
				caption << "SEARCH";
				apply_surface(0, 0, background, screen);
				message = TTF_RenderText_Solid(font, caption.str().c_str(), textColor);
				apply_surface((SCREEN_WIDTH - message->w) / 2, SCREEN_HEIGHT / 3 - message->h, message, screen);

				SDL_Flip(screen);
				if(SDL_PollEvent(&event)) {
				if(event.type == SDL_KEYDOWN) {
					switch(event.key.keysym.sym) {
						case SDLK_a:
							id[id_count] = 'a';
							id_count++;
						case SDLK_b:
							id[id_count] = 'b';
							id_count++;

						case SDLK_c:
							id[id_count] = 'c';
							id_count++;

						case SDLK_d:
							id[id_count] = 'd';
							id_count++;

						case SDLK_e:
							id[id_count] = 'e';
							id_count++;

						case SDLK_f:
							id[id_count] = 'f';
							id_count++;

						case SDLK_g:
							id[id_count] = 'g';
							id_count++;

						case SDLK_h:
							id[id_count] = 'h';
							id_count++;
						case SDLK_i:
							id[id_count] = 'i';
							id_count++;
						case SDLK_j:
							id[id_count] = 'j';
							id_count++;

						case SDLK_k:
							id[id_count] = 'k';
							id_count++;

						case SDLK_l:
							id[id_count] = 'l';
							id_count++;

						case SDLK_m:
							id[id_count] = 'm';
							id_count++;

						case SDLK_n:
							id[id_count] = 'n';
							id_count++;

						case SDLK_o:
							id[id_count] = 'o';
							id_count++;

						case SDLK_p:
							id[id_count] = 'p';
							id_count++;
						case SDLK_q:
							id[id_count] = 'q';
							id_count++;
						case SDLK_r:
							id[id_count] = 'r';
							id_count++;

						case SDLK_s:
							id[id_count] = 's';
							id_count++;

						case SDLK_t:
							id[id_count] = 't';
							id_count++;

						case SDLK_u:
							id[id_count] = 'u';
							id_count++;

						case SDLK_v:
							id[id_count] = 'v';
							id_count++;

						case SDLK_w:
							id[id_count] = 'w';
							id_count++;

						case SDLK_x:
							id[id_count] = 'x';
							id_count++;
						case SDLK_y:
							id[id_count] = 'y';
							id_count++;
						case SDLK_z:
							id[id_count] = 'z';
							id_count++;

						////////숫자 입력받음

						case SDLK_0:
							id[id_count] = 't';
							id_count++;

						case SDLK_1:
							id[id_count] = 'u';
							id_count++;

						case SDLK_2:
							id[id_count] = 'v';
							id_count++;

						case SDLK_3:
							id[id_count] = 'w';
							id_count++;

						case SDLK_4:
							id[id_count] = 'x';
							id_count++;
						case SDLK_5:
							id[id_count] = 'y';
							id_count++;
						case SDLK_6:
							id[id_count] = 'z';
							id_count++;
						case SDLK_7:
							id[id_count] = 'x';
							id_count++;
						case SDLK_8:
							id[id_count] = 'y';
							id_count++;
						case SDLK_9:
							id[id_count] = 'z';
							id_count++;

					}
				}//if(event.type == SDL_KEYDOWN)의 괄호 닫기 (AAAAAAAA적혀있는)
				}//while문의 괄호 닫
				}//if(event.type == SDL_KEYDOWN)의 괄호 닫기 (make id적혀있는)

				}//case sdlk_space 괄호 닫음



}
	return INITIAL_MODE;
}
