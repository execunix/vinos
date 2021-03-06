# $NetBSD: Makefile,v 1.1.2.2 2015/03/03 08:28:12 snj Exp $

MODULE=	r300

CPPFLAGS+=	-DCOMPILE_R300 -DR200_MERGED=0 \
		-DRADEON_COMMON=1 -DRADEON_COMMON_FOR_R300 \
		-DRADEON_R300
CPPFLAGS+=	-DHAVE_LIBDRM_RADEON

CPPFLAGS+=	-I${X11SRCDIR.MesaLib7}/src/mesa/drivers/dri/r300 \
		-I${X11SRCDIR.MesaLib7}/src/mesa/drivers/dri/r300/compiler \
		-I${X11SRCDIR.MesaLib7}/src/mesa/drivers/dri/radeon \
		-I${X11SRCDIR.MesaLib7}/src/mesa/drivers/dri/radeon/server \
		-I${X11SRCDIR.MesaLib7}/src/glsl

DRI_EXTRA_PATHS=	${X11SRCDIR.MesaLib7}/src/mesa/drivers/dri/radeon \
			${X11SRCDIR.MesaLib7}/src/mesa/drivers/dri/r300/compiler \
			${X11SRCDIR.MesaLib7}/src/glsl \
			${X11SRCDIR.MesaLib7}/src/mesa/program

SRCS=	\
	radeon_bo_legacy.c \
	radeon_buffer_objects.c \
	radeon_common_context.c \
	radeon_common.c \
	radeon_cs_legacy.c \
	radeon_dma.c \
	radeon_debug.c \
	radeon_fbo.c \
	radeon_lock.c \
	radeon_mipmap_tree.c \
	radeon_pixel_read.c \
	radeon_queryobj.c \
	radeon_span.c \
	radeon_texture.c \
	radeon_tex_copy.c \
	radeon_tex_getimage.c \
	radeon_tile.c

SRCS+=	\
		 radeon_screen.c \
		 r300_blit.c \
		 r300_context.c \
		 r300_draw.c \
		 r300_cmdbuf.c \
		 r300_state.c \
		 r300_render.c \
		 r300_tex.c \
		 r300_texstate.c \
		 r300_vertprog.c \
		 r300_fragprog_common.c \
		 r300_shader.c \
		 radeon_mesa_to_rc.c \
		 r300_emit.c \
		 r300_swtcl.c

SRCS+= \
	radeon_cs_space_drm.c \
	radeon_bo.c \
	radeon_cs.c

# r300/compiler
SRCS+= \
		radeon_code.c \
		radeon_compiler.c \
		radeon_compiler_util.c \
		radeon_emulate_branches.c \
		radeon_emulate_loops.c \
		radeon_program.c \
		radeon_program_print.c \
		radeon_opcodes.c \
		radeon_program_alu.c \
		radeon_program_pair.c \
		radeon_program_tex.c \
		radeon_pair_translate.c \
		radeon_pair_schedule.c \
		radeon_pair_regalloc.c \
		radeon_pair_dead_sources.c \
		radeon_dataflow.c \
		radeon_dataflow_deadcode.c \
		radeon_dataflow_swizzles.c \
		radeon_list.c \
		radeon_optimize.c \
		radeon_remove_constants.c \
		radeon_rename_regs.c \
		radeon_variable.c \
		r3xx_fragprog.c \
		r300_fragprog.c \
		r300_fragprog_swizzle.c \
		r300_fragprog_emit.c \
		r500_fragprog.c \
		r500_fragprog_emit.c \
		r3xx_vertprog.c \
		r3xx_vertprog_dump.c \
		memory_pool.c

# glsl
SRCS+=	ralloc.c

# mesa/program
SRCS+=	register_allocate.c

LIBDPLIBS+=	drm_radeon	${.CURDIR}/../../libdrm_radeon

# uses C99 variable decl scoping
CFLAGS+=	-std=c99

CWARNFLAGS.clang+=	-Wno-empty-body -Wno-format -Wno-constant-conversion \
			-Wno-constant-logical-operand

.include "${.CURDIR}/../dri.mk"
