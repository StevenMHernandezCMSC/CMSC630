total_time = 129006800.000000
time_per_process = {
    '0.load_image': 3889499.000000,
    '1.grayscale': 2790106.000000,
    '2.noise__salt_and_pepper': 20196355.000000,
    '3.noise__gaussian': 43546945.000000,
    '4.create_histogram': 1691590.000000,
    '5.histogram_equalization': 3367171.000000,
    '6.uniform_quantization': 2835766.000000,
    '7.non_uniform_quantization': 2759985.000000,
    '8.linear_filter': 16031221.000000,
    '9.median_filter': 30140853.000000,
    '10.create_histogram': 1647161.000000,
}

#
#
#

micros_per_millis = 1000
millis_per_second = 1000
num_images = 500

#
# Time total
#

print("""
\\begin{tabular}{|l|r|r|}
\\hline
Pre-processing Step & Time(s) & Time(ms)  \\\\
\\hline
\\hline
""")

for k in time_per_process.keys():
    s = k.split(".")[1]
    s = s.replace("_", " ")
    s = s.title()
    print("{} & {:.5} & {:.5} \\\\\n\\hline".format(s, time_per_process[k] / micros_per_millis / millis_per_second, time_per_process[k] / micros_per_millis / num_images))

print("\\hline\nTotal time & {:.5} & {:.5} \\\\\n\\hline".format(total_time / micros_per_millis / millis_per_second, total_time / micros_per_millis / num_images))

print("""
\\end{tabular}
""")
