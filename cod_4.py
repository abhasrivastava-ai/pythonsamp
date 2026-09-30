import sys
import math
import pygame

# Initialize Pygame and Mixer for Audio
pygame.init()
pygame.mixer.init()

# Display settings
# We render internally at 320x240 for retro feel and performance, then scale up
RENDER_WIDTH = 320
RENDER_HEIGHT = 240
HALF_HEIGHT = RENDER_HEIGHT // 2
SCREEN_WIDTH = 640
SCREEN_HEIGHT = 480
FPS = 60

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
render_surface = pygame.Surface((RENDER_WIDTH, RENDER_HEIGHT))
pygame.display.set_caption("Textured 3D Raycaster with Audio")
clock = pygame.time.Clock()

# --- ASSET LOADING & GENERATION ---

# 1. AUDIO: Try to load sound files, fail gracefully if they don't exist
try:
    sfx_shoot = pygame.mixer.Sound("shoot.wav")
    sfx_hit = pygame.mixer.Sound("hit.wav")
except FileNotFoundError:
    sfx_shoot = None
    sfx_hit = None
    print("Audio files 'shoot.wav' / 'hit.wav' not found. Playing silently. Add them to your folder to hear sound!")

# 2. TEXTURES: Generate a procedural brick texture so no external images are required
TEX_SIZE = 64
wall_texture = pygame.Surface((TEX_SIZE, TEX_SIZE))
for x in range(TEX_SIZE):
    for y in range(TEX_SIZE):
        # Checkerboard brick pattern
        if (x // 16) % 2 == (y // 16) % 2:
            wall_texture.set_at((x, y), (139, 69, 19)) # Brown
        else:
            wall_texture.set_at((x, y), (160, 82, 45)) # Sienna
# Add mortar lines
pygame.draw.rect(wall_texture, (50, 50, 50), (0, 0, TEX_SIZE, TEX_SIZE), 2)
pygame.draw.line(wall_texture, (50, 50, 50), (0, 32), (64, 32), 2)

# To use your own image, delete the generation block above and uncomment this:
# wall_texture = pygame.image.load("wall.png").convert()
# TEX_SIZE = wall_texture.get_width()

# Map Configuration
MAP_SIZE = 16
GRID_MAP = [
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,
    1,0,1,0,1,0,1,1,1,1,0,1,0,1,0,1,
    1,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,
    1,0,1,1,1,1,0,1,0,1,1,1,0,1,0,1,
    1,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,
    1,1,1,0,0,1,0,1,1,1,0,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,
    1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,1,
    1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,
    1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,
    1,1,1,0,1,0,1,1,0,1,0,1,1,1,1,1,
    1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
    1,0,1,1,1,0,1,0,1,1,1,1,1,1,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
]

# Player State
player_x, player_y = 2.5, 2.5
player_dir_x, player_dir_y = -1.0, 0.0
player_plane_x, player_plane_y = 0.0, 0.66  # Controls FOV
player_hp, player_score = 100, 0

# Game State
recoil, is_shooting, game_over = 0, False, False
z_buffer = [0.0] * RENDER_WIDTH

enemies = [
    {"x": 8.5, "y": 3.5, "hp": 3},
    {"x": 13.5, "y": 7.5, "hp": 3},
    {"x": 4.5, "y": 13.5, "hp": 3},
    {"x": 10.5, "y": 11.5, "hp": 3}
]

pygame.mouse.set_visible(False)
pygame.event.set_grab(True)
font = pygame.font.SysFont("monospace", 24, bold=True)

def shoot():
    global recoil, is_shooting, player_score
    recoil = 12
    is_shooting = True
    
    if sfx_shoot:
        sfx_shoot.play()

    # Hitscan
    closest_dist = 999.0
    hit_index = -1
    for i, enemy in enumerate(enemies):
        dx, dy = enemy["x"] - player_x, enemy["y"] - player_y
        angle_to_enemy = math.atan2(dy, dx)
        player_angle = math.atan2(player_dir_y, player_dir_x)
        diff = (angle_to_enemy - player_angle + math.pi) % (2 * math.pi) - math.pi

        dist = math.hypot(dx, dy)
        if abs(diff) < 0.2 and dist < closest_dist:
            closest_dist = dist
            hit_index = i

    if hit_index != -1:
        if sfx_hit:
            sfx_hit.play()
        enemies[hit_index]["hp"] -= 1
        if enemies[hit_index]["hp"] <= 0:
            enemies.pop(hit_index)
            player_score += 100

running = True
while running:
    # 1. INPUT HANDLING
    for event in pygame.event.get():
        if event.type == pygame.QUIT or (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE):
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1 and not game_over:
            shoot()

    if not game_over:
        # Mouse Aim
        mouse_dx, _ = pygame.mouse.get_rel()
        rot_speed = -mouse_dx * 0.003
        old_dir_x = player_dir_x
        player_dir_x = player_dir_x * math.cos(rot_speed) - player_dir_y * math.sin(rot_speed)
        player_dir_y = old_dir_x * math.sin(rot_speed) + player_dir_y * math.cos(rot_speed)
        old_plane_x = player_plane_x
        player_plane_x = player_plane_x * math.cos(rot_speed) - player_plane_y * math.sin(rot_speed)
        player_plane_y = old_plane_x * math.sin(rot_speed) + player_plane_y * math.cos(rot_speed)

        # Movement
        keys = pygame.key.get_pressed()
        move_speed = 0.06
        
        if keys[pygame.K_w]:
            if GRID_MAP[int(player_y) * MAP_SIZE + int(player_x + player_dir_x * move_speed)] == 0:
                player_x += player_dir_x * move_speed
            if GRID_MAP[int(player_y + player_dir_y * move_speed) * MAP_SIZE + int(player_x)] == 0:
                player_y += player_dir_y * move_speed
        if keys[pygame.K_s]:
            if GRID_MAP[int(player_y) * MAP_SIZE + int(player_x - player_dir_x * move_speed)] == 0:
                player_x -= player_dir_x * move_speed
            if GRID_MAP[int(player_y - player_dir_y * move_speed) * MAP_SIZE + int(player_x)] == 0:
                player_y -= player_dir_y * move_speed
        if keys[pygame.K_a]:
            # Strafe left
            strafe_x = player_dir_y
            strafe_y = -player_dir_x
            if GRID_MAP[int(player_y) * MAP_SIZE + int(player_x + strafe_x * move_speed)] == 0: player_x += strafe_x * move_speed
            if GRID_MAP[int(player_y + strafe_y * move_speed) * MAP_SIZE + int(player_x)] == 0: player_y += strafe_y * move_speed
        if keys[pygame.K_d]:
            # Strafe right
            strafe_x = -player_dir_y
            strafe_y = player_dir_x
            if GRID_MAP[int(player_y) * MAP_SIZE + int(player_x + strafe_x * move_speed)] == 0: player_x += strafe_x * move_speed
            if GRID_MAP[int(player_y + strafe_y * move_speed) * MAP_SIZE + int(player_x)] == 0: player_y += strafe_y * move_speed

        # Enemy Logic
        for enemy in enemies:
            edx, edy = player_x - enemy["x"], player_y - enemy["y"]
            dist = math.hypot(edx, edy)
            if dist > 0.5:
                enemy["x"] += (edx / dist) * 0.015
                enemy["y"] += (edy / dist) * 0.015
            else:
                player_hp -= 0.5
                if player_hp <= 0: game_over = True

        if recoil > 0: recoil -= 1

    # 2. RENDERING (To low-res internal surface)
    render_surface.fill((17, 17, 17), (0, 0, RENDER_WIDTH, HALF_HEIGHT)) # Ceiling
    render_surface.fill((34, 34, 34), (0, HALF_HEIGHT, RENDER_WIDTH, HALF_HEIGHT)) # Floor

    # True DDA Raycaster
    for x in range(RENDER_WIDTH):
        camera_x = 2 * x / RENDER_WIDTH - 1
        ray_dir_x = player_dir_x + player_plane_x * camera_x
        ray_dir_y = player_dir_y + player_plane_y * camera_x

        map_x, map_y = int(player_x), int(player_y)
        delta_dist_x = abs(1 / ray_dir_x) if ray_dir_x != 0 else 1e30
        delta_dist_y = abs(1 / ray_dir_y) if ray_dir_y != 0 else 1e30

        if ray_dir_x < 0:
            step_x, side_dist_x = -1, (player_x - map_x) * delta_dist_x
        else:
            step_x, side_dist_x = 1, (map_x + 1.0 - player_x) * delta_dist_x

        if ray_dir_y < 0:
            step_y, side_dist_y = -1, (player_y - map_y) * delta_dist_y
        else:
            step_y, side_dist_y = 1, (map_y + 1.0 - player_y) * delta_dist_y

        hit, side = 0, 0
        while hit == 0:
            if side_dist_x < side_dist_y:
                side_dist_x += delta_dist_x
                map_x += step_x
                side = 0
            else:
                side_dist_y += delta_dist_y
                map_y += step_y
                side = 1
            if GRID_MAP[map_y * MAP_SIZE + map_x] > 0: hit = 1

        if side == 0: perp_wall_dist = side_dist_x - delta_dist_x
        else: perp_wall_dist = side_dist_y - delta_dist_y

        z_buffer[x] = perp_wall_dist
        line_height = int(RENDER_HEIGHT / (perp_wall_dist + 0.0001))
        draw_start = max(0, -line_height // 2 + HALF_HEIGHT)
        draw_end = min(RENDER_HEIGHT, line_height // 2 + HALF_HEIGHT)

        # Calculate exact Texture X coordinate
        if side == 0: wall_x = player_y + perp_wall_dist * ray_dir_y
        else: wall_x = player_x + perp_wall_dist * ray_dir_x
        wall_x -= math.floor(wall_x)

        tex_x = int(wall_x * TEX_SIZE)
        if (side == 0 and ray_dir_x > 0) or (side == 1 and ray_dir_y < 0):
            tex_x = TEX_SIZE - tex_x - 1

        # Extract texture column, scale to line_height, and blit to screen
        if draw_end > draw_start:
            tex_col = wall_texture.subsurface((tex_x, 0, 1, TEX_SIZE))
            
            # Simple darkness shading based on side
            if side == 1: 
                darken = pygame.Surface((1, TEX_SIZE))
                darken.set_alpha(100)
                darken.fill((0,0,0))
                tex_col = tex_col.copy()
                tex_col.blit(darken, (0,0))

            scaled_col = pygame.transform.scale(tex_col, (1, draw_end - draw_start))
            render_surface.blit(scaled_col, (x, draw_start))

    # Render Enemies (Sprites)
    sorted_enemies = sorted(enemies, key=lambda e: (player_x - e["x"])**2 + (player_y - e["y"])**2, reverse=True)
    for enemy in sorted_enemies:
        ex, ey = enemy["x"] - player_x, enemy["y"] - player_y
        inv_det = 1.0 / (player_plane_x * player_dir_y - player_dir_x * player_plane_y)
        transform_x = inv_det * (player_dir_y * ex - player_dir_x * ey)
        transform_y = inv_det * (-player_plane_y * ex + player_plane_x * ey)

        if transform_y > 0:
            sprite_screen_x = int((RENDER_WIDTH / 2) * (1 + transform_x / transform_y))
            sprite_height = abs(int(RENDER_HEIGHT / transform_y))
            draw_start_y = max(0, -sprite_height // 2 + HALF_HEIGHT)
            draw_end_y = min(RENDER_HEIGHT - 1, sprite_height // 2 + HALF_HEIGHT)
            
            draw_start_x = max(0, -sprite_height // 2 + sprite_screen_x)
            draw_end_x = min(RENDER_WIDTH - 1, sprite_height // 2 + sprite_screen_x)

            color = (255, 50, 50) if enemy["hp"] < 3 else (255, 0, 85)
            for stripe in range(draw_start_x, draw_end_x):
                if 0 <= stripe < RENDER_WIDTH and transform_y < z_buffer[stripe]:
                    pygame.draw.line(render_surface, color, (stripe, draw_start_y), (stripe, draw_end_y))

    # 3. SCALE TO SCREEN & DRAW UI
    pygame.transform.scale(render_surface, (SCREEN_WIDTH, SCREEN_HEIGHT), screen)

    # UI Overlay (Drawn at full resolution)
    gun_x = SCREEN_WIDTH // 2 - 20
    gun_y = SCREEN_HEIGHT - 80 + recoil
    pygame.draw.rect(screen, (68, 68, 68), (gun_x, gun_y, 40, 80))
    pygame.draw.rect(screen, (34, 34, 34), (gun_x + 15, gun_y - 20, 10, 20))

    if is_shooting and recoil > 6:
        pygame.draw.circle(screen, (255, 255, 0), (SCREEN_WIDTH // 2, gun_y - 20), 20)
    is_shooting = False

    # Crosshair
    pygame.draw.line(screen, (0, 255, 0), (SCREEN_WIDTH // 2 - 10, SCREEN_HEIGHT//2), (SCREEN_WIDTH // 2 + 10, SCREEN_HEIGHT//2), 2)
    pygame.draw.line(screen, (0, 255, 0), (SCREEN_WIDTH // 2, SCREEN_HEIGHT//2 - 10), (SCREEN_WIDTH // 2, SCREEN_HEIGHT//2 + 10), 2)

    hud_text = font.render(f"HEALTH: {int(player_hp)} | SCORE: {player_score}", True, (0, 255, 0))
    screen.blit(hud_text, (20, 20))

    if game_over:
        over_text = font.render("GAME OVER", True, (255, 0, 0))
        screen.blit(over_text, over_text.get_rect(center=(SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2)))

    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()
sys.exit()