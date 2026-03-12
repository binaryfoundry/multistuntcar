
#ifndef _OPPONENT_BEHAVIOUR
#define _OPPONENT_BEHAVIOUR

/*    ========= */
/*    Constants */
/*    ========= */
#define NO_OPPONENT (-1)

/*    ===================== */
/*    Structure definitions */
/*    ===================== */

/*    ============================== */
/*    External function declarations */
/*    ============================== */
extern void OpponentBehaviour(long* x, long* y, long* z, float* x_angle, float* y_angle, float* z_angle,
                              bool bOpponentPaused, float stepSeconds);

extern void CarToCarCollision(void);
extern void QueueMultiplayerCarCollisionImpulse(long instanceIndex, long xAcceleration, long yAcceleration,
                                                long zAcceleration, bool collided);
extern void ClearMultiplayerCarCollisionImpulses(void);

extern long CalculateIfWinning(long start_finish_piece);

extern long CalculateOpponentsDistance(void);
extern void GetOpponentRoadState(long* piece, long* distanceIntoSection);

extern void CapturePreviousOpponentShadow(void);

extern void UpdateInterpolatedOpponentShadow(float alpha);

#endif /* _OPPONENT_BEHAVIOUR */
