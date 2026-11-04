"""
Battery Life Calculator - Revamped Propane Tank
Uses measured current draw and usage assumptions from the Performance Review.
"""

# --- User Input ---
measured_current_mA = float(input("Enter measured system current draw (mA): "))

# --- Assumptions (from Performance Review) ---
uses_per_year = 30          # times propane tank is used per year
checks_per_use = 4          # times user checks gauge per use
on_time_per_check_s = 30    # seconds the system is on per check
battery_capacity_mAh = 2000 # 4x AA usable capacity at low drain
solar_fraction = 0.5        # fraction of uses where solar supplies all power
buck_efficiency = 0.85      # LM2596 buck converter efficiency
safety_factor = 5           # accounts for self-discharge, aging, temp, etc.

# --- Calculations ---
# Actual current from battery (accounting for buck converter efficiency)
current_from_battery_mA = measured_current_mA / buck_efficiency

# Charge per check (mAh)
on_time_hours = on_time_per_check_s / 3600
charge_per_check_mAh = current_from_battery_mA * on_time_hours

# Total checks per year
checks_per_year = uses_per_year * checks_per_use

# Yearly charge consumption (only battery-powered fraction)
battery_fraction = 1 - solar_fraction
yearly_charge_mAh = charge_per_check_mAh * checks_per_year * battery_fraction

# Battery life
ideal_life_years = battery_capacity_mAh / yearly_charge_mAh
conservative_life_years = ideal_life_years / safety_factor

# --- Output ---
print("\n===== RESULTS =====")
print(f"Measured current:          {measured_current_mA:.1f} mA")
print(f"Current from battery:      {current_from_battery_mA:.1f} mA (after buck efficiency)")
print(f"Charge per check:          {charge_per_check_mAh:.4f} mAh")
print(f"Checks per year:           {checks_per_year}")
print(f"Yearly battery drain:      {yearly_charge_mAh:.4f} mAh")
print(f"Ideal battery life:        {ideal_life_years:.1f} years")
print(f"Conservative battery life: {conservative_life_years:.1f} years (with {safety_factor}x safety factor)")

print("\n===== ASSUMPTIONS =====")
print(f"Uses per year:             {uses_per_year}")
print(f"Checks per use:            {checks_per_use}")
print(f"On time per check:         {on_time_per_check_s} seconds")
print(f"Battery capacity:          {battery_capacity_mAh} mAh")
print(f"Solar fraction:            {solar_fraction*100:.0f}%")
print(f"Buck converter efficiency: {buck_efficiency*100:.0f}%")
print(f"Safety factor:             {safety_factor}x")