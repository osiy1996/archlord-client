
class AgcdUIOption {
public:
	/*
	 * Initializes internal state.
	 */
	static int Init();
	
	/*
	 * Enables or disables auto-attack option.
	 */
	static void SetAutoAttackOption(int enabled);

	/*
	 * Forcefully changes auto attack option.
	 */
	static void ForceAutoAttackOption(int enabled);

	/*
	 * Enables or disables receive experience option.
	 */
	static void SetReceiveExperienceOption(int enabled);

	/*
	 * Forcefully changes receive experience option.
	 */
	static void ForceReceiveExperienceOption(int enabled);

	/*
	 * Is auto-attack enabled?
	 */
	static int IsAutoAttackEnabled();
};