// #include "kernel.h"

// void scheduler( ctx_t* ctx ) {
//   if      ( current == &pcb[ 0 ] ) {
//     memcpy( &pcb[ 0 ].ctx, ctx, sizeof( ctx_t ) );
//     memcpy( ctx, &pcb[ 1 ].ctx, sizeof( ctx_t ) );
//     current = &pcb[ 1 ];
//   }
//   else if ( current == &pcb[ 1 ] ) {
//     memcpy( &pcb[ 1 ].ctx, ctx, sizeof( ctx_t ) );
//     memcpy( ctx, &pcb[ 2 ].ctx, sizeof( ctx_t ) );
//     current = &pcb[ 2 ];
//   } else if ( current == &pcb[ 2 ] ) {
//     memcpy( &pcb[ 2 ].ctx, ctx, sizeof( ctx_t ) );
//     memcpy( ctx, &pcb[ 0 ].ctx, sizeof( ctx_t ) );
//     current = &pcb[ 0 ];
//   }
// }
